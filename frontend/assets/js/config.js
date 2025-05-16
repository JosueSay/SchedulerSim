async function initializeConfig() {
  const hasRouteAccess = await checkRouteAccess("/config");
  if (!hasRouteAccess) return;
  // Configuración inicial si tiene acceso.
}

function onAlgorithmChange() {
  const algorithm = document.getElementById("algorithm").value;
  const quantumConfig = document.getElementById("quantum-config");
  quantumConfig.style.display = algorithm === "RR" ? "flex" : "none";
}

function startSimulation() {
  const config = {
    algorithm: document.getElementById("algorithm").value,
    quantum: document.getElementById("quantum").value || null,
    mode: document.querySelector('input[name="mode"]:checked').value,
    cycles: document.getElementById("cycles").value,
  };

  console.log("Configuración seleccionada:", config);
  alert("Simulación configurada. (Integrar con backend próximamente)");
}
