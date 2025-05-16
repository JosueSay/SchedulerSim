function initializeIndex() {
  ["procesos", "recursos", "acciones"].forEach((type) =>
    loadFilePreview(type, `${type}.txt`)
  );
}

async function uploadAll() {
  const files = {
    procesos: document.getElementById("procesos").files[0],
    recursos: document.getElementById("recursos").files[0],
    acciones: document.getElementById("acciones").files[0],
  };

  if (!files.procesos || !files.recursos || !files.acciones) {
    alert("Debes seleccionar los 3 archivos antes de subir.");
    return;
  }

  const formData = new FormData();
  formData.append("files", files.procesos);
  formData.append("files", files.recursos);
  formData.append("files", files.acciones);

  const response = await fetch("/uploadFiles/", {
    method: "POST",
    body: formData,
  });

  if (response.ok) {
    allowAccess(); // Habilita acceso a /config
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
