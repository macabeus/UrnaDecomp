// Every piece of state the URL fragment carries, one parser per key, shared by the pages.
import { parseAsInteger, parseAsString, parseAsStringLiteral } from 'nuqs';

export const VIEWS = ['progress', 'playground', 'benchmark', 'urnaemu'] as const;
export type View = (typeof VIEWS)[number];
export const viewParser = parseAsStringLiteral(VIEWS).withDefault('progress');

/** the progress page's drill-down: an origin, then one of its components */
export const originParser = parseAsStringLiteral(['tse', 'upstream', 'unattributed'] as const);
export const componentParser = parseAsString;
/** what the progress page counts: functions or code bytes */
export const metricParser = parseAsStringLiteral(['functions', 'bytes'] as const).withDefault('functions');
/** the order of the progress page's function waffle */
export const SORTS = ['verdict', 'index', 'size'] as const;
export type Sort = (typeof SORTS)[number];
export const sortParser = parseAsStringLiteral(SORTS).withDefault('verdict');

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
