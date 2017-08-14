[%bs.raw {|require('./App.css')|}];

external logo : string = "./logo.svg" [@@bs.module];

type state = {
  showInput: bool,
  currencyToUpdate: string,
  amount: int
};

let component = ReasonReact.statefulComponent "App";

let make _children => {
  let onAmountChange event {ReasonReact.state: state} => {
    let value = (ReactDOMRe.domElementToObj (ReactEventRe.Form.target event))##value;
    ReasonReact.Update {...state, amount: value}
  };
  let onCurrencyChange event {ReasonReact.state: state} => {
    let value = (ReactDOMRe.domElementToObj (ReactEventRe.Form.target event))##value;
    ReasonReact.Update {...state, currencyToUpdate: value}
  };
  let saveAmount _ {ReasonReact.state: state} => {
    Dom.Storage.setItem
      state.currencyToUpdate (string_of_int state.amount) Dom.Storage.localStorage;
    ReasonReact.Update {...state, showInput: false}
  };
  let cancel _ {ReasonReact.state: state} => ReasonReact.Update {...state, showInput: false};
  let toggleInput _ {ReasonReact.state: state} => ReasonReact.Update {...state, showInput: true};
  {
    ...component,
    initialState: fun _ => {showInput: false, currencyToUpdate: "SIGT", amount: 0},
    render: fun {state, update, enqueue} => {
      Js.Promise.(
        GetCurrentIsk.getCurrentUsdValueForIsk () |> then_ (fun res => Js.log res |> resolve)
      );
      Js.log state;
      <div className="App">
        <div className="App-header">
          (
            state.showInput ?
              <div>
                <input onChange=(update onAmountChange) />
                <select value=state.currencyToUpdate onChange=(update onCurrencyChange)>
                  <option value="SIGT"> (ReasonReact.stringToElement "SIGT") </option>
                  <option value="BTC"> (ReasonReact.stringToElement "BTC") </option>
                </select>
                <button onClick=(enqueue saveAmount)>
                  (ReasonReact.stringToElement "Save")
                </button>
                <button onClick=(enqueue cancel)> (ReasonReact.stringToElement "Cancel") </button>
              </div> :
              <button onClick=(enqueue toggleInput)>
                (ReasonReact.stringToElement "Change amount")
              </button>
          )
        </div>
      </div>
    }
  }
};
