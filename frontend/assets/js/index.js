function redirigirSimulacion() {
  const seleccion = document.getElementById("tipo-simulacion").value;
  if (seleccion === "scheduling") {
    window.location.href = "/upload-scheduling";
  } else if (seleccion === "synchronization") {
    window.location.href = "/upload-synchronization";
  }
}
