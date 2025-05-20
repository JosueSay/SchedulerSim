/**
 * Redirige al usuario a la vista correspondiente según el tipo de simulación seleccionado.
 * Lee el valor del elemento con ID `tipo-simulacion` y redirige a la ruta adecuada:
 * - "/upload-scheduling" si la opción es "scheduling".
 * - "/upload-synchronization" si la opción es "synchronization".
 *
 * @return {void}
 */
function redirigirSimulacion() {
  const seleccion = document.getElementById("tipo-simulacion").value;
  if (seleccion === "scheduling") {
    window.location.href = "/upload-scheduling";
  } else if (seleccion === "synchronization") {
    window.location.href = "/upload-synchronization";
  }
}
