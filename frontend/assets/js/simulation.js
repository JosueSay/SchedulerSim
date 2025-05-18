// ==============================
// 🎨 Configuración de Colores
// ==============================
const colorPalette = [
  "#e63946",
  "#a8dadc",
  "#457b9d",
  "#1d3557",
  "#ffb703",
  "#fb8500",
  "#8ecae6",
  "#219ebc",
  "#023047",
  "#f1faee",
];
const processColors = {};

function getProcessColor(pid) {
  if (!processColors[pid]) {
    const index = Object.keys(processColors).length % colorPalette.length;
    processColors[pid] = colorPalette[index];
  }
  return processColors[pid];
}

// ==============================
// 📊 Actualización de Métricas
// ==============================
function updateMetricsPanel(metrics) {
  document.getElementById("metrics-panel").innerHTML = `
    <div id="metricas" class="card">
      <div class="metric-box">🕒 Tiempo de Espera Promedio: <span>${metrics[
        "Average Waiting Time"
      ].toFixed(2)}</span></div>
      <div class="metric-box">🔁 Tiempo de Retorno Promedio: <span>${metrics[
        "Average Turnaround Time"
      ].toFixed(2)}</span></div>
      <div class="metric-box">⏱️ Tiempo de Respuesta Promedio: <span>${metrics[
        "Average Response Time"
      ].toFixed(2)}</span></div>
    </div>
  `;
}

// ==============================
// 📅 Renderizado del Gantt (Tabla)
// ==============================
function renderGanttTable(events) {
  const table = document.getElementById("gantt-table");
  const legend = document.getElementById("gantt-legend");
  table.innerHTML = "";
  legend.innerHTML = "";

  const pids = [...new Set(events.map((e) => e.pid))];
  const maxCycle = Math.max(...events.map((e) => e.endCycle));

  pids.forEach((pid) => {
    const color = getProcessColor(pid);
    legend.innerHTML += `<div class="legend-item"><div class="color-box" style="background:${color}"></div>${pid}</div>`;
  });

  let headerRow = "<tr><th>Proceso</th>";
  for (let c = 0; c <= maxCycle; c++) headerRow += `<th>${c}</th>`;
  headerRow += "</tr>";
  table.innerHTML += headerRow;

  pids.forEach((pid) => {
    let row = `<tr><td>${pid}</td>`;
    for (let c = 0; c <= maxCycle; c++) {
      const event = events.find(
        (e) => e.pid === pid && c >= e.startCycle && c < e.endCycle
      );
      row += event
        ? `<td style="background:${getProcessColor(
            pid
          )}; color: #000;">${pid}</td>`
        : "<td></td>";
    }
    row += "</tr>";
    table.innerHTML += row;
  });
}

// ==============================
// 🎬 Simulación en Tiempo Real con WebSockets
// ==============================
let ws;
function initializeSimulation() {
  const config = JSON.parse(localStorage.getItem("lastSimulationConfig")) || {
    algorithm: "FIFO",
  };
  ws = new WebSocket("ws://127.0.0.1:8000/ws/simulation");

  const cycleCounter = document.getElementById("cycle-counter");
  let currentCycle = 0;
  const events = [];

  ws.onopen = () => {
    console.log("Conexión WebSocket establecida.");
    ws.send(JSON.stringify(config));
  };

  ws.onmessage = (event) => {
    const data = JSON.parse(event.data);

    if (data.event === "SIMULATION_END") {
      ws.close();
      cycleCounter.textContent = `Simulación Finalizada. Último Ciclo: ${currentCycle}`;
      return;
    }

    if (data.metrics) {
      updateMetricsPanel(data.metrics);
    } else if (data.pid) {
      currentCycle = data.endCycle;
      events.push(data);
      renderGanttTable(events);
      cycleCounter.textContent = `Ciclo Actual: ${currentCycle}`;
    }
  };

  ws.onerror = (err) => {
    console.error("Error en WebSocket:", err);
    showAlert("Error", "Conexión con la simulación perdida.", "error");
  };

  ws.onclose = () => {
    console.log("Conexión WebSocket cerrada.");
  };
}

function pauseSimulation() {
  if (ws) ws.send(JSON.stringify({ command: "pause" }));
}

function resumeSimulation() {
  if (ws) ws.send(JSON.stringify({ command: "resume" }));
}

function resetSimulation() {
  document.getElementById("cycle-counter").textContent = "Ciclo Actual: 0";
  const table = document.getElementById("gantt-table");
  Array.from(table.getElementsByTagName("td")).forEach((cell) => {
    cell.style.backgroundColor = "";
    cell.style.color = "";
  });
  if (ws) ws.close();
  initializeSimulation();
}

// ==============================
// 🚀 Navegación
// ==============================
function restartSimulation() {
  denyAccess();
  window.location.href = "/config";
}

function returnHome() {
  denyAccess();
  window.location.href = "/";
}
