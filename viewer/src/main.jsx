import React, { useEffect, useState } from "react";
import { createRoot } from "react-dom/client";
import ExcalidrawLib from "@excalidraw/excalidraw/dist/excalidraw.production.min.js";
import "./styles.css";

const { Excalidraw } = ExcalidrawLib;

function getMkdocsTheme() {
  return document.body.getAttribute("data-md-color-scheme") === "slate" ? "dark" : "light";
}

function ExcalidrawEmbed({ src }) {
  const [scene, setScene] = useState(null);
  const [theme, setTheme] = useState(getMkdocsTheme());
  const [isFullscreen, setIsFullscreen] = useState(false);

  useEffect(() => {
    fetch(src)
      .then((r) => r.json())
      .then(setScene)
      .catch((err) => console.error("Failed to load excalidraw scene", src, err));
  }, [src]);

  useEffect(() => {
    const observer = new MutationObserver(() => setTheme(getMkdocsTheme()));
    observer.observe(document.body, {
      attributes: true,
      attributeFilter: ["data-md-color-scheme"],
    });
    return () => observer.disconnect();
  }, []);

  useEffect(() => {
    if (!isFullscreen) return;
    const onKeyDown = (e) => {
      if (e.key === "Escape") setIsFullscreen(false);
    };
    document.addEventListener("keydown", onKeyDown);
    const prevOverflow = document.body.style.overflow;
    document.body.style.overflow = "hidden";
    return () => {
      document.removeEventListener("keydown", onKeyDown);
      document.body.style.overflow = prevOverflow;
    };
  }, [isFullscreen]);

  if (!scene) {
    return <div className="excalidraw-loading">Loading drawing…</div>;
  }

  return (
    <div
      className={
        isFullscreen ? "excalidraw-embed-inner excalidraw-embed-inner--fullscreen" : "excalidraw-embed-inner"
      }
    >
      <button
        type="button"
        className="excalidraw-fullscreen-toggle"
        onClick={() => setIsFullscreen((v) => !v)}
        title={isFullscreen ? "Exit fullscreen" : "Fullscreen"}
        aria-label={isFullscreen ? "Exit fullscreen" : "Fullscreen"}
      >
        {isFullscreen ? "✕" : "⛶"}
      </button>
      <Excalidraw
        key={isFullscreen}
        initialData={{
          elements: scene.elements,
          appState: { ...scene.appState, theme },
          scrollToContent: true,
        }}
        viewModeEnabled
        theme={theme}
      />
    </div>
  );
}

function assetBase() {
  const script = Array.from(document.getElementsByTagName("script")).find((s) =>
    s.src.includes("excalidraw-viewer.js")
  );
  return script ? script.src.replace(/assets\/js\/excalidraw-viewer\.js.*$/, "") : "";
}

function mount(el) {
  const src = assetBase() + el.dataset.src;
  createRoot(el).render(<ExcalidrawEmbed src={src} />);
}

function init() {
  window.EXCALIDRAW_ASSET_PATH = assetBase() + "assets/js/";
  document.querySelectorAll(".excalidraw-embed:not([data-mounted])").forEach((el) => {
    el.setAttribute("data-mounted", "true");
    mount(el);
  });
}

if (document.readyState === "loading") {
  document.addEventListener("DOMContentLoaded", init);
} else {
  init();
}
