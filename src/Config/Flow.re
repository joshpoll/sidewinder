module MS = Belt.Map.String;

/* TODO: use sets? */

type linear = MS.t(list(Place.t));

type general = list((list(Place.t), list(Place.t)));