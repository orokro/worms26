const fs = require('fs');
const path = require('path');

// Configuration
const INPUT_FILE = 'Game/SpriteData.c';
const OUTPUT_DIR = 'SpriteBitmaps';

// Ensure output directory exists
if (!fs.existsSync(OUTPUT_DIR)) {
    fs.mkdirSync(OUTPUT_DIR, { recursive: true });
}

// Helper: Create a BMP buffer from pixel data (0 = white, 1 = black)
function createBMP(width, height, data) {
    const rowSize = Math.ceil((width * 3) / 4) * 4; // Row size padded to 4 bytes
    const fileSize = 54 + rowSize * height; // Header (54) + Pixel Data
    const buffer = Buffer.alloc(fileSize);

    // --- Bitmap File Header (14 bytes) ---
    buffer.write('BM', 0);             // Signature
    buffer.writeUInt32LE(fileSize, 2); // File size
    buffer.writeUInt32LE(54, 10);      // Offset to pixel data

    // --- DIB Header (40 bytes) ---
    buffer.writeUInt32LE(40, 14);      // Header size
    buffer.writeInt32LE(width, 18);    // Width
    buffer.writeInt32LE(-height, 22);  // Height (negative = top-down)
    buffer.writeUInt16LE(1, 26);       // Planes
    buffer.writeUInt16LE(24, 28);      // Bits per pixel (RGB)
    
    // --- Pixel Data ---
    let offset = 54;
    for (let y = 0; y < height; y++) {
        for (let x = 0; x < width; x++) {
            const isBlack = data[y][x] === 1;
            const color = isBlack ? 0x00 : 0xFF; // 0x00=Black, 0xFF=White
            
            // Write BGR (Blue, Green, Red)
            buffer.writeUInt8(color, offset + x * 3 + 0);
            buffer.writeUInt8(color, offset + x * 3 + 1);
            buffer.writeUInt8(color, offset + x * 3 + 2);
        }
        // Padding is handled by jumping offset by rowSize
        offset += rowSize; // Move to next row start
        // Actually, since we write sequentially, we just need to add padding bytes
        const padding = rowSize - (width * 3);
        offset -= rowSize; // Reset to start of row (logic fix)
        offset += (width * 3); // Move past pixels
        for(let p=0; p<padding; p++) buffer.writeUInt8(0, offset++);
    }

    return buffer;
}

// Helper: Determine width from C type
function getWidthFromType(type) {
    if (type.includes('char')) return 8;
    if (type.includes('short')) return 16;
    if (type.includes('long')) return 32;
    return 8; // default
}

// Main logic
try {
    const content = fs.readFileSync(INPUT_FILE, 'utf8');

    // Regex to find variable definitions. 
    // Matches: static [type] [name][dims] = { ... };
    // We capture: Type, Name, Dims, and Body (the content inside curly braces)
    const regex = /static\s+(unsigned\s+(?:char|short|long))\s+([a-zA-Z0-9_]+)(\[[^\]]*\])?(\[[^\]]*\])?\s*=\s*\{([\s\S]*?)\};/g;

    let match;
    while ((match = regex.exec(content)) !== null) {
        const type = match[1];
        const name = match[2];
        const body = match[5];
        const width = getWidthFromType(type);

        console.log(`Processing ${name} (${type}, width: ${width})...`);

        // Check if 2D array (contains nested braces)
        if (body.includes('{')) {
            // It's a 2D array (like spr_weapons or spr_Mountain)
            // Split by inner sets of braces
            const subMatches = body.match(/\{[\s\S]*?\}/g);
            
            if (subMatches) {
                subMatches.forEach((subBlock, index) => {
                    const cleanBlock = subBlock.replace(/[{}]/g, '');
                    const rows = parseDataBlock(cleanBlock, width);
                    if (rows.length > 0) {
                        const filename = `${name}_${index}.bmp`;
                        const bmp = createBMP(width, rows.length, rows);
                        fs.writeFileSync(path.join(OUTPUT_DIR, filename), bmp);
                        console.log(`  -> Saved ${filename}`);
                    }
                });
            }
        } else {
            // Standard 1D array
            const rows = parseDataBlock(body, width);
            if (rows.length > 0) {
                const filename = `${name}.bmp`;
                const bmp = createBMP(width, rows.length, rows);
                fs.writeFileSync(path.join(OUTPUT_DIR, filename), bmp);
                console.log(`  -> Saved ${filename}`);
            }
        }
    }

} catch (err) {
    console.error("Error:", err.message);
}

function parseDataBlock(text, width) {
    // Remove comments
    text = text.replace(/\/\/.*$/gm, '');
    
    // Split by commas
    const lines = text.split(',').map(l => l.trim()).filter(l => l);
    
    const pixels = []; // Array of rows (which are arrays of 0/1)

    lines.forEach(line => {
        // Look for 0b binary literal
        const binMatch = line.match(/0b([01]+)/);
        if (binMatch) {
            let binStr = binMatch[1];
            // Pad or trim to fit width
            binStr = binStr.padStart(width, '0').slice(-width);
            
            const row = binStr.split('').map(Number); // [0, 1, 1, ...]
            pixels.push(row);
        }
    });

    return pixels;
}