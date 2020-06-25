/* https://dev.to/margaretkrutikova/reason-react-context-explained-in-action-5eki */

type state =
  | Before
  | After;

type animationState = {
  curr: state,
  next: state,
};

type animationAction =
  | ToBefore
  | ToAfter;

type dispatch = animationAction => unit;

type contextValue = animationState;

let initValue: contextValue = {curr: Before, next: Before};

let context = React.createContext(initValue);

let useAnimation = () => React.useContext(context);