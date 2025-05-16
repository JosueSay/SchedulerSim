async function initializeConfig() {
  const hasRouteAccess = await checkRouteAccess("/config");
  if (!hasRouteAccess) return;
}

function onAlgorithmChange() {
  const algorithm = document.getElementById("algorithm").value;
  document.getElementById("quantum-config").style.display =
    algorithm === "RR" ? "flex" : "none";
  document.getElementById("preemptive-config").style.display =
    algorithm === "PS" ? "flex" : "none";
}

function startSimulation() {
  const config = {
    algorithm: document.getElementById("algorithm").value,
    quantum: document.getElementById("quantum").value || null,
    isPreemptive:
      document.querySelector('input[name="preemptive"]:checked')?.value || 0,
    startCycle: parseInt(document.getElementById("startCycle").value),
    maxCycles: parseInt(document.getElementById("maxCycles").value),
  };

  console.log("[DEBUG] Configuración de Simulación:", config);
  alert("Simulación configurada. (Falta integrar con Backend)");
  // TODO: Enviar al backend cuando esté implementado.
}
