module MS = Belt.Map.String;

/* TODO: use sets? */

type linear = list((Place.t, list(Place.t)));

type linearExt = {
  pattern: linear,
  extFn: linear,
};

/* currently unsupported */
type general = list((list(Place.t), list(Place.t)));

let none = {pattern: [], extFn: []};
