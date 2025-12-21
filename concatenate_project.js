import fs from 'fs';
import path from 'path';


// --- CONFIGURATION ---
const OUTPUT_FILE = 'full_project_context.txt';

// Folders to strictly ignore
const IGNORED_DIRS = new Set([
  'node_modules',
  '.git',
  '.vscode',
  'dist',
  'build',
  'coverage',
  'shops_and_misc', // Binary source files (Blender/PSD)
  'public',         // Binary assets (GLB, PNG, MP3)
  'html',
  'Resources',
  
]);

// Files to strictly ignore (optional, for specific large files)
const IGNORED_FILES = new Set([
  'package-lock.json',
  'yarn.lock',
  '.DS_Store'
]);

// Extensions we WANT to read.
// If a file is in 'src' but is a .png, it will still be skipped.
const ALLOWED_EXTENSIONS = new Set([
  '.js',
  '.mjs',
  '.vue',
  '.json',
  '.html',
  '.css',
  '.scss',
  '.md',
  '.txt',
  '.c',
  '.h',
]);

// --- MAIN SCRIPT ---

function processDirectory(currentPath) {
  const entries = fs.readdirSync(currentPath, { withFileTypes: true });

  for (const entry of entries) {
    const fullPath = path.join(currentPath, entry.name);

    // 1. Handle Directories
    if (entry.isDirectory()) {
      if (!IGNORED_DIRS.has(entry.name)) {
        processDirectory(fullPath);
      }
    }
    // 2. Handle Files
    else if (entry.isFile()) {
      const ext = path.extname(entry.name).toLowerCase();

      // Check if file is allowed and not explicitly ignored
      if (ALLOWED_EXTENSIONS.has(ext) && !IGNORED_FILES.has(entry.name)) {
        try {
          const content = fs.readFileSync(fullPath, 'utf8');
          appendToFile(fullPath, content);
          console.log(`Included: ${fullPath}`);
        } catch (err) {
          console.error(`Error reading ${fullPath}: ${err.message}`);
        }
      }
    }
  }
}

function appendToFile(filePath, content) {
  const header = `\n\n================================================================================\nFile: ${filePath}\n================================================================================\n`;
  fs.appendFileSync(OUTPUT_FILE, header + content);
}

// --- EXECUTION ---

try {
  // Clear existing output file if it exists
  if (fs.existsSync(OUTPUT_FILE)) {
    fs.unlinkSync(OUTPUT_FILE);
  }

  console.log('Scanning project...');
  processDirectory('.');
  console.log(`\nDone! Context saved to: ${OUTPUT_FILE}`);
} catch (err) {
  console.error('Fatal Error:', err);
}
