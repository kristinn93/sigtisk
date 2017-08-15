open Bs_fetch;

type remoteFunc;

external make : unit => remoteFunc = "" [@@bs.module "electron"];

Js.log (make ());

let foo () => [%bs.raw {|
  window.require('electron').remote.require('./foo.js')()
|}];

Js.log (foo ());

module Crypto = {
  type results = {symbol: string};
  let decodeCurrency json => Json.Decode.{symbol: json |> field "Symbol" string};
  let getCurrentValues () =>
    Js.Promise.(
      fetchWithInit
        "https://www.cryptopia.co.nz/Exchange/GetListData?_=1502053453124"
        (
          RequestInit.make
            method_::Get
            mode::NoCORS
            headers::(
              HeadersInit.make {"Accept": "application/json", "Content-Type": "application/json"}
            )
            ()
        ) |>
      /* then_ Response.json |> */
      then_ (fun res => Js.log res |> resolve)
    );
};

module Isk = {
  type results = {
    shortName: string,
    value: float
  };
  type currencyResult = {results: array results};
  let decodeCurrency json =>
    Json.Decode.{shortName: json |> field "shortName" string, value: json |> field "value" float};
  let decodeResults json => Json.Decode.(json |> field "results" (array decodeCurrency));
  let getCurrentUsdValue () =>
    Js.Promise.(
      fetch "https://apis.is/currency/arion" |> then_ Response.json |>
      then_ (fun res => decodeResults res |> resolve) |>
      then_ (
        fun res =>
          (
            Array.of_list (
              List.filter
                (fun item => List.exists (fun currency => currency == item.shortName) ["USD"])
                (Array.to_list res)
            )
          ).(0).
            value |> resolve
      )
    );
};
