async function initializeSyncConfig() {
  const hasAccess = await checkRouteAccess("/config-synchronization");
  if (!hasAccess) return;
}

async function startSyncSimulation() {
  const mechanism = document.getElementById("mechanism").value;
  const config = { mechanism };
  localStorage.setItem("lastSyncConfig", JSON.stringify(config));
  window.location.href = "/simulation-synchronization";
}
