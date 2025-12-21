const fs = require('fs');
const path = require('path');

// Configuration
const INPUT_FILE = 'Game/SpriteData.c';
const OUTPUT_DIR = 'SpriteBitmaps';

// Ensure output directory exists
if (!fs.existsSync(OUTPUT_DIR)) {
    fs.mkdirSync(OUTPUT_DIR, { recursive: true });
}

// Helper: Create a BMP buffer
function createBMP(width, height, data) {
    const rowSize = Math.ceil((width * 3) / 4) * 4;
    const fileSize = 54 + rowSize * height;
    const buffer = Buffer.alloc(fileSize);

    buffer.write('BM', 0);
    buffer.writeUInt32LE(fileSize, 2);
    buffer.writeUInt32LE(54, 10);
    buffer.writeUInt32LE(40, 14);
    buffer.writeInt32LE(width, 18);
    buffer.writeInt32LE(-height, 22);
    buffer.writeUInt16LE(1, 26);
    buffer.writeUInt16LE(24, 28);
    
    let offset = 54;
    for (let y = 0; y < height; y++) {
        for (let x = 0; x < width; x++) {
            const isBlack = data[y][x] === 1;
            const color = isBlack ? 0x00 : 0xFF;
            buffer.writeUInt8(color, offset + x * 3 + 0);
            buffer.writeUInt8(color, offset + x * 3 + 1);
            buffer.writeUInt8(color, offset + x * 3 + 2);
        }
        offset += rowSize;
        offset -= rowSize;
        offset += (width * 3);
        const padding = rowSize - (width * 3);
        for(let p=0; p<padding; p++) buffer.writeUInt8(0, offset++);
    }
    return buffer;
}

// Helper: Get width based on type
function getWidthFromType(type) {
    if (type.includes('char')) return 8;
    if (type.includes('short')) return 16;
    if (type.includes('long')) return 32;
    return 32; // Default fallback
}

// Helper: Extract data from binary strings
function parseBinaryBlock(text, width) {
    const pixels = [];
    // Remove comments
    const cleanText = text.replace(/\/\/.*$/gm, '');
    // Match 0b followed by 0s and 1s
    const matches = cleanText.match(/0b[01]+/g);
    
    if (matches) {
        matches.forEach(binStr => {
            // Remove '0b' prefix
            let bits = binStr.substring(2);
            // Pad or Trim to width
            bits = bits.padStart(width, '0').slice(-width);
            pixels.push(bits.split('').map(Number));
        });
    }
    return pixels;
}

try {
    console.log(`Reading ${INPUT_FILE}...`);
    let content = fs.readFileSync(INPUT_FILE, 'utf8');

    // 1. Sanitize Input
    content = content.replace(/\u00A0/g, ' ').replace(/[^\x00-\x7F]/g, ' ');

    // 2. Regex to find the START of a declaration
    // UPDATED: Made "static" optional using (?:static\s+)?
    const startRegex = /(?:static\s+)?(?:const\s+)?(unsigned\s+(?:char|short|long))\s+([a-zA-Z0-9_]+)[^=]*=\s*\{/g;

    let match;
    let count = 0;

    while ((match = startRegex.exec(content)) !== null) {
        const type = match[1];
        const name = match[2];
        const startIndex = match.index;
        const width = getWidthFromType(type);

        console.log(`Found sprite: ${name} (${type})`);

        // 3. Brace Counting to find the full body
        let openCount = 0;
        let bodyStartIndex = -1;
        let bodyEndIndex = -1;
        
        for (let i = startIndex; i < content.length; i++) {
            if (content[i] === '{') {
                if (openCount === 0) bodyStartIndex = i;
                openCount++;
            } else if (content[i] === '}') {
                openCount--;
                if (openCount === 0) {
                    bodyEndIndex = i + 1;
                    break;
                }
            }
        }

        if (bodyStartIndex !== -1 && bodyEndIndex !== -1) {
            const fullBody = content.substring(bodyStartIndex, bodyEndIndex);
            
            // Check for 2D array
            const innerContent = fullBody.substring(1, fullBody.length - 1);
            if (innerContent.includes('{')) {
                // 2D Array Logic
                let subSpriteIndex = 0;
                let subOpen = 0;
                let subStart = -1;

                for(let j=0; j<innerContent.length; j++) {
                    if (innerContent[j] === '{') {
                        if (subOpen === 0) subStart = j;
                        subOpen++;
                    } else if (innerContent[j] === '}') {
                        subOpen--;
                        if (subOpen === 0) {
                            const subBlock = innerContent.substring(subStart, j+1);
                            const pixels = parseBinaryBlock(subBlock, width);
                            if (pixels.length > 0) {
                                const filename = `${name}_${subSpriteIndex}.bmp`;
                                fs.writeFileSync(path.join(OUTPUT_DIR, filename), createBMP(width, pixels.length, pixels));
                                console.log(`   -> Saved ${filename}`);
                                subSpriteIndex++;
                                count++;
                            }
                        }
                    }
                }
            } else {
                // 1D Array Logic
                const pixels = parseBinaryBlock(fullBody, width);
                if (pixels.length > 0) {
                    const filename = `${name}.bmp`;
                    fs.writeFileSync(path.join(OUTPUT_DIR, filename), createBMP(width, pixels.length, pixels));
                    console.log(`   -> Saved ${filename}`);
                    count++;
                }
            }
        } else {
            console.warn(`   !! Could not find closing brace for ${name}`);
        }
    }
    console.log(`\nDone! Extracted ${count} sprite files to /${OUTPUT_DIR}`);

} catch (err) {
    console.error("Error:", err);
}