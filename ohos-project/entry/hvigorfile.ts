import { hapTasks } from '@ohos/hvigor-ohos-plugin';
import { HvigorPlugin, HvigorNode } from '@ohos/hvigor';
import * as fs from 'fs';
import * as path from 'path';

function assetSyncPlugin(): HvigorPlugin {
    return {
        pluginId: 'asset-sync-plugin',
        // This hook runs as soon as the module is recognized,
        // before any tasks (like resource compilation) are even created.
        apply(node: HvigorNode) {
            const sourceDir = path.resolve(node.getNodePath(), '../../bin/assets');
            const targetDir = path.resolve(node.getNodePath(), './src/main/resources/rawfile');

            console.info(`[AssetSync] Starting sync...`);

            if (fs.existsSync(sourceDir)) {
                if (!fs.existsSync(targetDir)) {
                    fs.mkdirSync(targetDir, { recursive: true });
                }

                const copyRecursive = (src: string, dest: string) => {
                    if (fs.statSync(src).isDirectory()) {
                        if (!fs.existsSync(dest)) fs.mkdirSync(dest);
                        fs.readdirSync(src).forEach(file => {
                            copyRecursive(path.join(src, file), path.join(dest, file));
                        });
                    } else {
                        // Using synchronous copy to ensure it finishes
                        // before Hvigor proceeds to the next build step
                        fs.copyFileSync(src, dest);
                    }
                };

                copyRecursive(sourceDir, targetDir);
                console.info(`[AssetSync] Successfully copied assets to rawfile.`);
            } else {
                console.warn(`[AssetSync] Source folder not found: ${sourceDir}`);
            }
        }
    }
}

export default {
    system: hapTasks,
    plugins: [
        assetSyncPlugin()
    ]
};