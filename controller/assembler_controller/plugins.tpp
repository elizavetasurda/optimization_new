// ============================================================
//  AUTOMATICALLY GENERATED FILE
//  DO NOT EDIT MANUALLY
// ============================================================

#include <string>
#include "plugins_controller.hpp"
#include "plugins/f_list.cpp"
#include "plugins/gold_sech.cpp"
#include "plugins/porabola.cpp"
#include "plugins/grad_spysk.cpp"
#include "plugins/conjugate_grad.cpp"
#include "plugins/bfgs.cpp"
#include "plugins/newton.cpp"
#include "plugins/hooke_jeeves.cpp"
#include "plugins/neldermead.cpp"
#include "plugins/lagrange.cpp"
#include "plugins/penalty.cpp"
#include "plugins/barrier.cpp"

template<typename T,typename Func>
void CollectPlug::collect(Func func, const std::string plugin_name) {
    if (plugin_name == "f_list.cpp" || plugin_name == "f_list") func(f_list<T>());
    else if (plugin_name == "gold_sech.cpp" || plugin_name == "gold_sech") func(gold_sech<T>());
    else if (plugin_name == "porabola.cpp" || plugin_name == "porabola") func(porabola<T>());
    // === ДОБАВЛЕННЫЕ МЕТОДЫ ===
    else if (plugin_name == "grad_spysk.cpp" || plugin_name == "grad_spysk") func(grad_spysk<T>());
    else if (plugin_name == "conjugate_grad.cpp" || plugin_name == "conjugate_grad") func(conjugate_grad<T>());
    else if (plugin_name == "bfgs.cpp" || plugin_name == "bfgs") func(bfgs<T>());
    else if (plugin_name == "newton.cpp" || plugin_name == "newton") func(newton<T>());
    else if (plugin_name == "hooke_jeeves.cpp" || plugin_name == "hooke_jeeves") func(hooke_jeeves<T>());
    else if (plugin_name == "neldermead.cpp" || plugin_name == "neldermead") func(neldermead<T>());
    else if (plugin_name == "lagrange.cpp" || plugin_name == "lagrange") func(lagrange<T>());
    else if (plugin_name == "penalty.cpp" || plugin_name == "penalty") func(penalty<T>());
    else if (plugin_name == "barrier.cpp" || plugin_name == "barrier") func(barrier<T>());
    // ===============================
}
