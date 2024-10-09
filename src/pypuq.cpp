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

py::array_t<double> array_to_numpy(puq::Array array) {
  py::array_t<double> numpy(array.value.size());
  py::buffer_info buf_info = numpy.request();
  double* ptr = static_cast<double*>(buf_info.ptr);
  std::memcpy(ptr, array.value.data(), array.value.size() * sizeof(double));;
  return numpy;
}

std::variant<double, std::vector<double>, py::array_t<double>> quantity_value(puq::Quantity q, bool numpy) {
  if (numpy) {
    return array_to_numpy(q.value.magnitude.value);
  } else if (q.value.magnitude.value.size()==1) {
    return q.value.magnitude.value[0];
  } else {
    return q.value.magnitude.value.value;
  }
}

std::variant<double, std::vector<double>, py::array_t<double>> quantity_error(puq::Quantity q, bool numpy) {
  if (numpy) {
    return array_to_numpy(q.value.magnitude.error);
  } else if (q.value.magnitude.value.size()==1) {
    return q.value.magnitude.error[0];
  } else {
    return q.value.magnitude.error.value;
  }
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
    py::class_<puq::Quantity>(m, "Quantity")
      .def(py::init<std::string>())
      .def(py::init<std::string, puq::SystemType>())
      .def(py::init<double>())
      .def(py::init<double, std::string>())
      .def(py::init<double, std::string, puq::SystemType>())
      .def(py::init<double, double>())
      .def(py::init<double, double, std::string>())
      .def(py::init<double, double, std::string, puq::SystemType>())
      .def(py::init<puq::ArrayValue>())
      .def(py::init<puq::ArrayValue, std::string>())
      .def(py::init<puq::ArrayValue, std::string, puq::SystemType>())
      .def(py::init<puq::ArrayValue, puq::ArrayValue>())
      .def(py::init<puq::ArrayValue, puq::ArrayValue, std::string>())
      .def(py::init<puq::ArrayValue, puq::ArrayValue, std::string, puq::SystemType>())
      .def("convert", py::overload_cast<std::string, puq::SystemType, const std::string&>(&puq::Quantity::convert, py::const_),
	   py::arg("expression"), py::arg("system")=puq::SystemType::NONE, py::arg("quantity")="")
      .def("convert", py::overload_cast<const puq::Dformat&, puq::SystemType>(&puq::Quantity::convert, py::const_),
	   py::arg("dformat"), py::arg("system")=puq::SystemType::NONE) 
      .def("unit_system", &puq::Quantity::unit_system)
      .def("rebase_prefixes", &puq::Quantity::rebase_prefixes)
      .def("rebase_dimensions", &puq::Quantity::rebase_dimensions)
      .def("to_string", &puq::Quantity::to_string, py::arg("precision") = 6)
      .def("size", &puq::Quantity::size)
      .def("value", &quantity_value, py::arg("numpy")=false)
      .def("error", &quantity_error, py::arg("numpy")=false)
      .def("__repr__", &puq::Quantity::to_string, py::arg("precision") = 6)
      .def("__str__", &puq::Quantity::to_string, py::arg("precision") = 6)
      .def(py::self + py::self)
      .def(py::self - py::self)
      .def(py::self * py::self)
      .def(py::self / py::self)
      .def(double() + py::self)
      .def(double() - py::self)
      .def(double() * py::self)
      .def(double() / py::self)
      .def(py::self + double())
      .def(py::self - double())
      .def(py::self * double())
      .def(py::self / double())
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
