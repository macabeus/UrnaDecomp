import { cpp } from '@codemirror/lang-cpp';
import { StreamLanguage } from '@codemirror/language';
import { wast } from '@codemirror/legacy-modes/mode/wast';
import { EditorState } from '@codemirror/state';
import { EditorView, lineNumbers } from '@codemirror/view';
import CodeMirror from '@uiw/react-codemirror';
import { useMemo, useState } from 'react';

import type { Wat } from '../../lib/target';

const wat = StreamLanguage.define(wast);
const readOnly = [EditorState.readOnly.of(true), EditorView.editable.of(false)];
const BASIC = { foldGutter: false, highlightActiveLine: false } as const;

function hex(n: number): string {
  return n.toString(16).padStart(6, '0');
}

/** The WAT with each line's body offset in the gutter instead of a line number. */
export function WatView({ value }: { value: Wat }) {
  const extensions = useMemo(
    () => [
      wat,
      ...readOnly,
      lineNumbers({
        formatNumber: (n) => {
          const o = value.offsets[n - 1];
          return o === null || o === undefined ? '' : hex(o);
        },
      }),
    ],
    [value],
  );
  return (
    <CodeMirror
      value={value.text}
      theme="dark"
      height="100%"
      className="cm-pane h-full"
      extensions={extensions}
      basicSetup={{ ...BASIC, lineNumbers: false }}
    />
  );
}

export function FoldView({ value }: { value: string }) {
  const extensions = useMemo(() => [cpp(), ...readOnly], []);
  return (
    <CodeMirror value={value} theme="dark" height="100%" className="cm-pane h-full" extensions={extensions} basicSetup={BASIC} />
  );
}

/** The lifted C++: editable, for reading and copying. Remounted (by key) when the function changes. */
export function CppEditor({ initial, fileName }: { initial: string; fileName: string }) {
  const [text, setText] = useState(initial);
  const [copied, setCopied] = useState(false);
  const extensions = useMemo(() => [cpp()], []);
  const edited = text !== initial;

  const copy = () => {
    void navigator.clipboard.writeText(text).then(() => {
      setCopied(true);
      setTimeout(() => setCopied(false), 1200);
    });
  };
  const download = () => {
    const url = URL.createObjectURL(new Blob([text], { type: 'text/x-c++src' }));
    const a = document.createElement('a');
    a.href = url;
    a.download = fileName;
    a.click();
    URL.revokeObjectURL(url);
  };

  return (
    <div className="flex h-full flex-col gap-1.5">
      <div className="flex items-center gap-1.5">
        <span className="text-xs text-slate-500">{edited ? 'edited in this tab' : 'as lifted'}</span>
        <div className="ml-auto flex gap-1">
          <button type="button" onClick={copy} className="rounded-md px-2 py-0.5 text-xs text-teal-300 hover:bg-slate-800">
            {copied ? 'copied' : 'Copy'}
          </button>
          <button type="button" onClick={download} className="rounded-md px-2 py-0.5 text-xs text-teal-300 hover:bg-slate-800">
            Download {fileName}
          </button>
          <button
            type="button"
            disabled={!edited}
            onClick={() => setText(initial)}
            className="rounded-md px-2 py-0.5 text-xs text-slate-300 hover:bg-slate-800 disabled:text-slate-600 disabled:hover:bg-transparent"
          >
            Reset
          </button>
        </div>
      </div>
      <div className="min-h-0 flex-1 overflow-hidden rounded-lg border border-slate-800">
        <CodeMirror
          value={text}
          onChange={setText}
          theme="dark"
          height="100%"
          className="cm-pane h-full"
          extensions={extensions}
          basicSetup={BASIC}
        />
      </div>
    </div>
  );
}
