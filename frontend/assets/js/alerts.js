if (!window.Swal) {
  const script = document.createElement("script");
  script.src = "https://cdn.jsdelivr.net/npm/sweetalert2@11";
  document.head.appendChild(script);
}

/**
 * Mostrar una alerta básica.
 * @param {string} title - Título de la alerta.
 * @param {string} text - Texto de la alerta.
 * @param {string} icon - Tipo de alerta: 'success', 'error', 'warning', 'info', 'question'.
 */
function showAlert(title, text, icon = "info") {
  Swal.fire({
    title: title,
    text: text,
    icon: icon,
    confirmButtonText: "Aceptar",
    confirmButtonColor: "#3498db",
  });
}

/**
 * Mostrar una confirmación con callbacks.
 * @param {string} title
 * @param {string} text
 * @param {function} onConfirm
 */
function showConfirm(title, text, onConfirm) {
  Swal.fire({
    title: title,
    text: text,
    icon: "question",
    showCancelButton: true,
    confirmButtonText: "Sí",
    cancelButtonText: "Cancelar",
    confirmButtonColor: "#3498db",
    cancelButtonColor: "#e74c3c",
  }).then((result) => {
    if (result.isConfirmed && typeof onConfirm === "function") {
      onConfirm();
    }
  });
}
