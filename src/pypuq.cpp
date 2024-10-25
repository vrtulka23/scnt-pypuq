#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include <pybind11/operators.h>
#include <variant>
#include <scnt-puq/quantity.h>
#include <scnt-puq/calc/calc.h>
#include <scnt-puq/systems/lists.h>

namespace py = pybind11;

// Adding Python context manager methods to UnitSystem
class UnitSystem: public puq::UnitSystem {
  using puq::UnitSystem::UnitSystem;
public:
  UnitSystem& enter() {return *this;}
  void exit(const py::object& exc_type, const py::object& exc_value, const py::object& traceback) {close();}
};

py::array_t<MAGNITUDE_PRECISION> array_to_numpy(puq::Array array) {
  py::array_t<MAGNITUDE_PRECISION> numpy(array.value.size());
  py::buffer_info buf_info = numpy.request();
  MAGNITUDE_PRECISION* ptr = static_cast<MAGNITUDE_PRECISION*>(buf_info.ptr);
  std::memcpy(ptr, array.value.data(), array.value.size() * sizeof(MAGNITUDE_PRECISION));
  numpy.resize(array.shape());
  return numpy;
}

std::variant<MAGNITUDE_PRECISION, std::vector<MAGNITUDE_PRECISION>, py::array_t<MAGNITUDE_PRECISION>> quantity_value(puq::Quantity q, bool numpy) {
  if (numpy) {
    return array_to_numpy(q.value.magnitude.value);
  } else if (q.value.magnitude.value.size()==1) {
    return q.value.magnitude.value[0];
  } else {
    return q.value.magnitude.value.value;
  }
}

std::variant<MAGNITUDE_PRECISION, std::vector<MAGNITUDE_PRECISION>, py::array_t<MAGNITUDE_PRECISION>> quantity_error(puq::Quantity q, bool numpy) {
  if (numpy) {
    return array_to_numpy(q.value.magnitude.error);
  } else if (q.value.magnitude.value.size()==1) {
    return q.value.magnitude.error[0];
  } else {
    return q.value.magnitude.error.value;
  }
}

puq::Array buffer_to_array(py::buffer_info& info) {
  puq::ArrayValue a(info.size);
  
  if (info.format != py::format_descriptor<MAGNITUDE_PRECISION>::format())
    throw std::runtime_error("Incompatible format: expected a double array!");
  
  std::copy(static_cast<MAGNITUDE_PRECISION*>(info.ptr),
	    static_cast<MAGNITUDE_PRECISION*>(info.ptr)+info.size,
	    a.begin());

  puq::ArrayShape s(info.shape.size());
  std::copy(info.shape.begin(), info.shape.end(), s.begin());
  return puq::Array(a, s);
}

puq::Quantity calculator(std::string e) {
  auto calc = puq::Calculator();
  auto atom = calc.solve(e);
  return atom.value;
}

PYBIND11_MODULE(pypuq, m) {

  m.doc() = "Physical Units and Quantities"; 

  m.attr("__version__") = CODE_VERSION;

  // Exposing all unit systems
  auto sys = m.def_submodule("systems", "Unit systems");
  auto e = py::enum_<puq::SystemType>(sys, "SystemType");
  for (auto sys: puq::SystemMap) {
    e.value(sys.second->SystemAbbrev.c_str(), sys.first);
  }
  e.export_values();

  // Exposing dimension formats
  auto df = m.def_submodule("formats", "Dimension formats");
  auto d = py::enum_<puq::Dformat>(df, "Dformat");
  d.value("MKS", puq::Dformat::MKS);
  d.value("MGS", puq::Dformat::MGS);
  d.value("CGS", puq::Dformat::CGS);
  d.value("FPS", puq::Dformat::FPS);
  d.export_values();

  // Exposing lists
  auto lists = m.def_submodule("lists", "Unit system lists");
  lists.def("prefixes", [](){py::print(puq::lists::prefixes());});
  lists.def("base_units", [](){py::print(puq::lists::base_units());});
  lists.def("derived_units", [](){py::print(puq::lists::derived_units());});
  lists.def("logarithmic_units", [](){py::print(puq::lists::logarithmic_units());});
  lists.def("temperature_units", [](){py::print(puq::lists::temperature_units());});
  lists.def("constants", [](){py::print(puq::lists::constants());});
  lists.def("quantities", [](){py::print(puq::lists::quantities());});
  lists.def("unit_systems", [](){py::print(puq::lists::unit_systems());});

  // Expose UnitSystem
  py::class_<puq::UnitSystem>(m, "UnitSystemBase")
    .def(py::init<const puq::SystemType>())
    .def("change", &puq::UnitSystem::change)      
    .def("close", &puq::UnitSystem::close)      
    ;

  py::class_<UnitSystem, puq::UnitSystem>(m, "UnitSystem")
    .def(py::init<const puq::SystemType>())
    .def("enter", &UnitSystem::enter)
    .def("exit", &UnitSystem::exit)  
    .def("__enter__", &UnitSystem::enter)
    .def("__exit__", &UnitSystem::exit)
    ;

  // Exposing calculator object
  m.def("Calculator", &calculator, py::arg("expression"));
    
  // Exposing quantity object
  py::class_<puq::Quantity>(m, "Quantity", py::buffer_protocol())
    .def(py::init<std::string>())
    .def(py::init<std::string, puq::SystemType>())
    .def(py::init<MAGNITUDE_PRECISION>())
    .def(py::init<MAGNITUDE_PRECISION, std::string>())
    .def(py::init<MAGNITUDE_PRECISION, std::string, puq::SystemType>())
    .def(py::init<MAGNITUDE_PRECISION, MAGNITUDE_PRECISION>())
    .def(py::init<MAGNITUDE_PRECISION, MAGNITUDE_PRECISION, std::string>())
    .def(py::init<MAGNITUDE_PRECISION, MAGNITUDE_PRECISION, std::string, puq::SystemType>())
    .def(py::init<puq::ArrayValue>())
    .def(py::init<puq::ArrayValue, std::string>())
    .def(py::init<puq::ArrayValue, std::string, puq::SystemType>())
    .def(py::init<puq::ArrayValue, puq::ArrayValue>())
    .def(py::init<puq::ArrayValue, puq::ArrayValue, std::string>())
    .def(py::init<puq::ArrayValue, puq::ArrayValue, std::string, puq::SystemType>())
    .def(py::init([](py::array_t<MAGNITUDE_PRECISION, py::array::c_style | py::array::forcecast> v,
		     std::string s, puq::SystemType) {
      py::buffer_info info = v.request();
      puq::Array value = buffer_to_array(info);
      return puq::Quantity(value, s);
    }), py::arg("value"), py::arg("unit")="", py::arg("system")=puq::SystemType::NONE)
    .def(py::init([](py::array_t<MAGNITUDE_PRECISION, py::array::c_style | py::array::forcecast> v,
		     py::array_t<MAGNITUDE_PRECISION, py::array::c_style | py::array::forcecast> e,
		     std::string s, puq::SystemType) {
      py::buffer_info v_info = v.request();
      puq::Array value = buffer_to_array(v_info);
      py::buffer_info e_info = e.request();
      puq::Array error = buffer_to_array(e_info);
      return puq::Quantity(value, error, s);
    }), py::arg("value"), py::arg("error"), py::arg("unit")="", py::arg("system")=puq::SystemType::NONE)
    .def_buffer([](puq::Quantity &q) -> py::buffer_info {
      return py::buffer_info(
			     q.value.magnitude.value.value.data(),
			     sizeof(MAGNITUDE_PRECISION),
			     py::format_descriptor<MAGNITUDE_PRECISION>::format(),
			     1,
			     q.value.magnitude.value.shape(),
			     {sizeof(MAGNITUDE_PRECISION)}
			     );
    })
    .def("convert", py::overload_cast<std::string, puq::SystemType, const std::string&>(&puq::Quantity::convert, py::const_),
	 py::arg("expression"), py::arg("system")=puq::SystemType::NONE, py::arg("quantity")="")
    .def("convert", py::overload_cast<const puq::Dformat&, puq::SystemType>(&puq::Quantity::convert, py::const_),
	 py::arg("dformat"), py::arg("system")=puq::SystemType::NONE) 
    .def("unit_system", &puq::Quantity::unit_system)
    .def("rebase_prefixes", &puq::Quantity::rebase_prefixes)
    .def("rebase_dimensions", &puq::Quantity::rebase_dimensions)
    .def("to_string", &puq::Quantity::to_string, py::arg("precision") = 6)
    .def("size", &puq::Quantity::size)
    .def("shape", &puq::Quantity::shape)
    .def("value", &quantity_value, py::arg("numpy")=false)
    .def("error", &quantity_error, py::arg("numpy")=false)
    .def("__repr__", &puq::Quantity::to_string, py::arg("precision") = 6)
    .def("__str__", &puq::Quantity::to_string, py::arg("precision") = 6)
    .def(py::self + py::self)
    .def(py::self - py::self)
    .def(py::self * py::self)
    .def(py::self / py::self)
    .def(MAGNITUDE_PRECISION() + py::self)
    .def(MAGNITUDE_PRECISION() - py::self)
    .def(MAGNITUDE_PRECISION() * py::self)
    .def(MAGNITUDE_PRECISION() / py::self)
    .def(py::self + MAGNITUDE_PRECISION())
    .def(py::self - MAGNITUDE_PRECISION())
    .def(py::self * MAGNITUDE_PRECISION())
    .def(py::self / MAGNITUDE_PRECISION())
    .def(puq::ArrayValue() + py::self)
    .def(puq::ArrayValue() - py::self)
    .def(puq::ArrayValue() * py::self)
    .def(puq::ArrayValue() / py::self)
    .def(py::self + puq::ArrayValue())
    .def(py::self - puq::ArrayValue())
    .def(py::self * puq::ArrayValue())
    .def(py::self / puq::ArrayValue())
    ;
    	
}
