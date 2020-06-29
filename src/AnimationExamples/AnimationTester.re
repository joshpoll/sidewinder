/* wrapper for animations so they can be triggered */
let leftButtonStyle = ReactDOMRe.Style.make(~borderRadius="4px 0px 0px 4px", ~width="48px", ());
let rightButtonStyle = ReactDOMRe.Style.make(~borderRadius="0px 4px 4px 0px", ~width="48px", ());

type state = {
  pos: int,
  length: int,
};

type action =
  | Increment
  | Decrement
  | Length(int)
  | Error;

let initialState = {pos: 0, length: 1};

let reducer = (state, action) => {
  switch (action) {
  | Increment => {...state, pos: min(state.length - 1, state.pos + 1)}
  | Decrement => {...state, pos: max(0, state.pos - 1)}
  | Length(length) => {...state, length}
  | Error => state
  };
};

let toggle = (AnimationComponentHelper.{curr, next}) =>
  switch (next) {
  | Before => AnimationComponentHelper.{curr: next, next: After}
  | After => {curr: next, next: Before}
  };

[@react.component]
let make = (~padding=10., ~trace) => {
  let (state, dispatch) = React.useReducer(reducer, initialState);
  let (AnimationComponentHelper.{curr, next}, setAnimationState) =
    React.useState(() => AnimationComponentHelper.initValue);

  // Notice that instead of `useEffect`, we have `useEffect0`. See
  // reasonml.github.io/reason-react/docs/en/components#hooks for more info
  React.useEffect0(() => {
    dispatch(Length(List.length(trace)));

    // Returning None, instead of Some(() => ...), means we don't have any
    // cleanup to do before unmounting. That's not 100% true. We should
    // technically cancel the promise. Unofortunately, there's currently no
    // way to cancel a promise. Promises in general should be way less used
    // for React components; but since folks do use them, we provide such an
    // example here. In reality, this fetch should just be a plain callback,
    // with a cancellation API
    None;
  });

  let swTrace = trace |> List.split |> (((flows, ns)) => Config.compile(flows, ns));
  let renderedConfig = List.nth(swTrace, state.pos);
  let width = 1000.;
  let height = 300.;
  let xOffset = 0.;
  let yOffset = 100.;
  <div>
    <div> {React.string("state: ")} {React.string(string_of_int(state.pos))} </div>
    <button
      style=leftButtonStyle
      onClick={_event => {
        dispatch(Decrement);
        setAnimationState(_ => AnimationComponentHelper.initValue);
      }}>
      {React.string("<-")}
    </button>
    <button onClick={_event => setAnimationState(toggle)}>
      {switch (curr) {
       | Before => React.string("To After")
       | After => React.string("To Before")
       }}
    </button>
    <button
      style=rightButtonStyle
      onClick={_event => {
        dispatch(Increment);
        setAnimationState(_ => AnimationComponentHelper.initValue);
      }}>
      {React.string("->")}
    </button>
    <svg
      xmlns="http://www.w3.org/2000/svg"
      width={Js.Float.toString(width +. padding *. 2.)}
      height={Js.Float.toString(height +. padding *. 2.)}>
      <g
        transform={
          "translate("
          ++ Js.Float.toString(xOffset +. padding)
          ++ ", "
          ++ Js.Float.toString(yOffset +. padding)
          ++ ")"
        }>
        <AnimationComponentProvider value=AnimationComponentHelper.{curr, next}>
          renderedConfig
        </AnimationComponentProvider>
      </g>
    </svg>
  </div>;
};
