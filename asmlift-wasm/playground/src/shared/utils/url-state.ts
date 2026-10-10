// Every piece of state the URL fragment carries, one parser per key, shared by both pages.
import { parseAsInteger, parseAsString, parseAsStringLiteral } from 'nuqs';

export const VIEWS = ['playground', 'benchmark'] as const;
export type View = (typeof VIEWS)[number];
export const viewParser = parseAsStringLiteral(VIEWS).withDefault('playground');

/** the function the playground shows */
export const fnParser = parseAsInteger;

export const PANES = ['wat', 'fold'] as const;
export const paneParser = parseAsStringLiteral(PANES).withDefault('wat');

/** a benchmark run's label; absent = the newest run */
export const runParser = parseAsString;
/** the run the benchmark page compares `run` against */
export const vsParser = parseAsString;
/** the benchmark page's setup filter */
export const setupParser = parseAsString;
/** the benchmark row whose details are open */
export const rowParser = parseAsString;
