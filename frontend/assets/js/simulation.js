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
// 🎬 Simulación Dinámica
// ==============================
let isPaused = false;
let simulationIterator = null;

async function* animateGanttChart(events) {
  const cycleCounter = document.getElementById("cycle-counter");
  let currentCycle = 0;

  for (const event of events) {
    for (let cycle = event.startCycle; cycle < event.endCycle; cycle++) {
      while (isPaused) await new Promise((resolve) => setTimeout(resolve, 100));
      currentCycle = cycle;
      cycleCounter.textContent = `Ciclo Actual: ${currentCycle}`;
      highlightGanttCell(event.pid, cycle);
      await new Promise((resolve) => setTimeout(resolve, 1000)); // 1 segundo por ciclo
      yield;
    }
  }
  cycleCounter.textContent = `Ciclo Actual: ${currentCycle + 1}`;
}

function highlightGanttCell(pid, cycle) {
  const table = document.getElementById("gantt-table");
  const rows = table.getElementsByTagName("tr");

  for (let i = 1; i < rows.length; i++) {
    const row = rows[i];
    if (row.cells[0].textContent === pid) {
      const cell = row.cells[cycle + 1];
      if (cell) {
        cell.style.backgroundColor = getProcessColor(pid);
        cell.style.color = "#000";
      }
    }
  }
}

// ==============================
// ▶️ Control de Simulación
// ==============================
async function initializeSimulation() {
  const response = await fetch("/simulate/", {
    method: "POST",
    headers: { "Content-Type": "application/json" },
    body: JSON.stringify({ algorithm: "FIFO" }),
  });

  if (response.ok) {
    const data = await response.json();
    updateMetricsPanel(data.metrics);
    renderGanttTable(data.timeline);

    const events = data.timeline;
    simulationIterator = animateGanttChart(events);
    simulationIterator.next();
  } else {
    showAlert("Error", "No se pudo cargar la simulación.", "error");
  }
}

function pauseSimulation() {
  isPaused = true;
}

function resumeSimulation() {
  if (simulationIterator) {
    isPaused = false;
    simulationIterator.next();
  }
}

function resetSimulation() {
  isPaused = false;
  document.getElementById("cycle-counter").textContent = "Ciclo Actual: 0";

  const table = document.getElementById("gantt-table");
  Array.from(table.getElementsByTagName("td")).forEach((cell) => {
    cell.style.backgroundColor = "";
    cell.style.color = "";
  });

  if (simulationIterator) simulationIterator = null;
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
