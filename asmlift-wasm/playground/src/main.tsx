import { StrictMode } from 'react';
import { createRoot } from 'react-dom/client';

import { App } from './App';
import './index.css';
import { NuqsHashAdapter } from './shared/utils/hash-adapter';

createRoot(document.getElementById('root')!).render(
  <StrictMode>
    <NuqsHashAdapter>
      <App />
    </NuqsHashAdapter>
  </StrictMode>,
);
