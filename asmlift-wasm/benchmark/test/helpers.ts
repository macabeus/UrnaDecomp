import { spawnSync } from 'node:child_process';

import { loadSetups } from '../src/setups';

export const IMAGE = loadSetups()[0].image;

export const hasImage = spawnSync('docker', ['image', 'inspect', IMAGE], { encoding: 'utf8' }).status === 0;
