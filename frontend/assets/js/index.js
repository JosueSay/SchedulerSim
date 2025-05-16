function initializeIndex() {
  console.log("[DEBUG] Entrando a '/', eliminando acceso previo.");
  denyAccess();

  ["procesos", "recursos", "acciones"].forEach((type) =>
    loadFilePreview(type, `${type}.txt`)
  );
}

function validateTxtContent(type, content) {
  const lines = content.trim().split("\n");
  const regexPatterns = {
    procesos: /^[A-Za-z0-9]+,\s*\d+,\s*\d+,\s*\d+$/,
    recursos: /^[A-Za-z0-9]+,\s*\d+$/,
    acciones: /^[A-Za-z0-9]+,\s*(READ|WRITE),\s*[A-Za-z0-9]+,\s*\d+$/,
  };

  const pattern = regexPatterns[type];
  if (!pattern) return false;

  for (const [index, line] of lines.entries()) {
    if (!pattern.test(line.trim())) {
      alert(
        `Error en ${type}.txt en la línea ${
          index + 1
        }:\n"${line.trim()}"\nCorrige el formato.`
      );
      return false;
    }
  }
  return true;
}

async function validateAndReadFile(file, type) {
  return new Promise((resolve) => {
    const reader = new FileReader();
    reader.onload = (e) => {
      const content = e.target.result;
      const isValid = validateTxtContent(type, content);
      resolve(isValid);
    };
    reader.readAsText(file);
  });
}

async function uploadAll() {
  const files = {
    procesos: document.getElementById("procesos").files[0],
    recursos: document.getElementById("recursos").files[0],
    acciones: document.getElementById("acciones").files[0],
  };

  // Validar existencia de los 3 archivos
  if (!files.procesos || !files.recursos || !files.acciones) {
    alert("Debes seleccionar los 3 archivos antes de subir.");
    return;
  }

  // Validar nombres correctos
  if (
    files.procesos.name.toLowerCase() !== "procesos.txt" ||
    files.recursos.name.toLowerCase() !== "recursos.txt" ||
    files.acciones.name.toLowerCase() !== "acciones.txt"
  ) {
    alert(
      "Los archivos deben llamarse exactamente:\n- procesos.txt\n- recursos.txt\n- acciones.txt"
    );
    return;
  }

  if (
    !files.procesos.name.toLowerCase().endsWith(".txt") ||
    !files.recursos.name.toLowerCase().endsWith(".txt") ||
    !files.acciones.name.toLowerCase().endsWith(".txt")
  ) {
    alert("Todos los archivos deben tener extensión .txt.");
    return;
  }

  // Validar Contenido de los Archivos
  const validations = await Promise.all([
    validateAndReadFile(files.procesos, "procesos"),
    validateAndReadFile(files.recursos, "recursos"),
    validateAndReadFile(files.acciones, "acciones"),
  ]);

  if (validations.includes(false)) return; // Detener si algún archivo es inválido

  const formData = new FormData();
  formData.append("files", files.procesos);
  formData.append("files", files.recursos);
  formData.append("files", files.acciones);

  const response = await fetch("/uploadFiles/", {
    method: "POST",
    body: formData,
  });

  if (response.ok) {
    allowAccess();
    window.location.href = "/config";
  } else {
    alert("Error al subir los archivos.");
  }
}

async function loadFilePreview(type, filename) {
  const response = await fetch("/listFiles/");
  const files = await response.json();
  const preview = files.find((f) => f.name === filename);
  const previewElement = document.getElementById(`preview-${type}`);
  if (previewElement) {
    previewElement.textContent = preview
      ? preview.preview
      : "Sin vista previa.";
  }
}
