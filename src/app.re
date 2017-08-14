[%bs.raw {|require('./App.css')|}];

external logo : string = "./logo.svg" [@@bs.module];

type state = {
  showInput: bool,
  currencyToUpdate: string,
  amount: float,
  intervalId: option Js.Global.intervalId,
  currentIsk: float
};

let component = ReasonReact.statefulComponent "App";

let make _children => {
  let onAmountChange event {ReasonReact.state: state} => {
    let value = (ReactDOMRe.domElementToObj (ReactEventRe.Form.target event))##value;
    ReasonReact.Update {...state, amount: float_of_string value}
  };
  let onCurrencyChange event {ReasonReact.state: state} => {
    let value = (ReactDOMRe.domElementToObj (ReactEventRe.Form.target event))##value;
    ReasonReact.Update {...state, currencyToUpdate: value}
  };
  let saveAmount _ {ReasonReact.state: state} => {
    Dom.Storage.setItem
      state.currencyToUpdate (string_of_float state.amount) Dom.Storage.localStorage;
    ReasonReact.Update {...state, showInput: false, amount: 0.}
  };
  let cancel _ {ReasonReact.state: state} => ReasonReact.Update {...state, showInput: false};
  let toggleInput _ {ReasonReact.state: state} => ReasonReact.Update {...state, showInput: true};
  let updateCurrency enqueue () =>
    Js.Promise.(
      Currencies.Isk.getCurrentUsdValue () |>
      then_ (
        fun res =>
          enqueue
            (fun _ {ReasonReact.state: state} => ReasonReact.Update {...state, currentIsk: res}) () |> resolve
      )
    ) |> ignore;
  {
    ...component,
    didMount: fun {enqueue, state} => {
      Js.Promise.(
        Currencies.Isk.getCurrentUsdValue () |>
        then_ (
          fun res =>
            enqueue
              (fun _ {ReasonReact.state: state} => ReasonReact.Update {...state, currentIsk: res})
              () |> resolve
        )
      ) |> ignore;
      ReasonReact.Update {
        ...state,
        intervalId: Some (Js.Global.setInterval (updateCurrency enqueue) (2 * 60 * 60 * 1000))
      }
    },
    initialState: fun _ => {
      showInput: false,
      currencyToUpdate: "SIGT",
      amount: 0.,
      intervalId: None,
      currentIsk: 0.
    },
    render: fun {state, update, enqueue} =>
      <div className="App">
        <span className="inspector" />
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
              <button className="changeamount" onClick=(enqueue toggleInput)>
                (ReasonReact.stringToElement "Change amount")
              </button>
          )
        </div>
        <div>
          <table className="stupidtable">
            <thead>
              <tr>
                <th> (ReasonReact.stringToElement "Currency") </th>
                <th> (ReasonReact.stringToElement "Value") </th>
                <th> (ReasonReact.stringToElement "Total ISK") </th>
              </tr>
            </thead>
            <tbody>
              <tr>
                <td> (ReasonReact.stringToElement "SIGT") </td>
                <td>
                  (
                    ReasonReact.stringToElement (
                      switch (Dom.Storage.getItem "SIGT" Dom.Storage.localStorage) {
                      | Some value => value
                      | None => string_of_float 0.
                      }
                    )
                  )
                </td>
                <td> (ReasonReact.stringToElement "SIGT") </td>
              </tr>
              <tr>
                <td> (ReasonReact.stringToElement "BTC") </td>
                <td>
                  (
                    ReasonReact.stringToElement (
                      switch (Dom.Storage.getItem "BTC" Dom.Storage.localStorage) {
                      | Some value => value
                      | None => string_of_float 0.
                      }
                    )
                  )
                </td>
                <td> (ReasonReact.stringToElement "SIGT") </td>
              </tr>
            </tbody>
          </table>
        </div>
      </div>
  }
};