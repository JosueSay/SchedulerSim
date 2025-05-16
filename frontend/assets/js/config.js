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

function validateSimulationConfig(config) {
  // Validar ciclo inicial
  if (config.startCycle < 0) {
    alert("El ciclo inicial no puede ser negativo.");
    return false;
  }

  // Validar ciclos máximos
  if (config.maxCycles < 0) {
    alert("Los ciclos máximos no pueden ser negativos.");
    return false;
  }

  // Validar quantum si es Round Robin
  if (config.algorithm === "RR") {
    if (!config.quantum || parseInt(config.quantum) <= 0) {
      alert("El quantum debe ser un número positivo para Round Robin.");
      return false;
    }
  }

  // Validar preemptive solo para Priority Scheduling (PS)
  if (config.algorithm !== "PS" && config.isPreemptive !== "0") {
    alert("El modo preventivo solo aplica para Priority Scheduling.");
    return false;
  }

  return true;
}

function startSimulation() {
  const config = {
    algorithm: document.getElementById("algorithm").value,
    quantum: document.getElementById("quantum").value || null,
    isPreemptive: document.getElementById("isPreemptive").value,
    startCycle: parseInt(document.getElementById("startCycle").value),
    maxCycles: parseInt(document.getElementById("maxCycles").value),
  };

  if (!validateSimulationConfig(config)) return;

  console.log("[DEBUG] Configuración de Simulación:", config);
  alert("Simulación configurada correctamente. (Falta integrar con Backend)");
  // TODO: Enviar al backend cuando esté implementado.
}
