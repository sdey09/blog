import { readFileSync, writeFileSync, mkdirSync, readdirSync, statSync } from "fs";
import path from "path";
import { fileURLToPath } from "url";
import lz from "lz-string";

const __dirname = path.dirname(fileURLToPath(import.meta.url));
const docsDir = path.join(__dirname, "..", "..", "docs");
const assetsDir = path.join(docsDir, "assets", "excalidraw");

const folders = [
  "deep-dives",
  "designing-data-intensive-application",
  "high-level-system-design-interview",
  "low-level-system-design-interview",
];

function slugify(name) {
  return name
    .toLowerCase()
    .replace(/[^a-z0-9]+/g, "-")
    .replace(/^-+|-+$/g, "");
}

function findMarkdownFiles(dir) {
  return readdirSync(dir)
    .filter((f) => f.endsWith(".md"))
    .filter((f) => statSync(path.join(dir, f)).isFile());
}

let processed = 0;
let skipped = 0;

for (const folder of folders) {
  const folderPath = path.join(docsDir, folder);
  const folderSlug = slugify(folder);
  const outDir = path.join(assetsDir, folderSlug);
  mkdirSync(outDir, { recursive: true });

  for (const file of findMarkdownFiles(folderPath)) {
    const filePath = path.join(folderPath, file);
    const text = readFileSync(filePath, "utf8");
    const match = text.match(/## Drawing\n```compressed-json\n([\s\S]*?)\n```/);

    if (!match) {
      console.log(`SKIP (no drawing data): ${folder}/${file}`);
      skipped++;
      continue;
    }

    const b64 = match[1].replace(/\n/g, "");
    const json = lz.decompressFromBase64(b64);
    if (!json) {
      console.log(`SKIP (failed to decompress): ${folder}/${file}`);
      skipped++;
      continue;
    }

    const scene = JSON.parse(json);
    const title = file.replace(/\.md$/, "");
    const fileSlug = slugify(title);
    const jsonPath = path.join(outDir, `${fileSlug}.json`);
    writeFileSync(jsonPath, JSON.stringify(scene));

    const dataSrc = `assets/excalidraw/${folderSlug}/${fileSlug}.json`;
    const newMd = `# ${title}\n\n<div class="excalidraw-embed" data-src="${dataSrc}"></div>\n`;
    writeFileSync(filePath, newMd);

    console.log(`OK: ${folder}/${file} -> ${dataSrc} (${scene.elements?.length ?? 0} elements)`);
    processed++;
  }
}

console.log(`\nDone. Processed ${processed}, skipped ${skipped}.`);
