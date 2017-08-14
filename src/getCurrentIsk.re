open Bs_fetch;

type results = {
  shortName: string,
  value: float
};

type currencyResult = {results: array results};

let decodeCurrency json =>
  Json.Decode.{shortName: json |> field "shortName" string, value: json |> field "value" float};

let decodeResults json => Json.Decode.(json |> field "results" (array decodeCurrency));

let getCurrentUsdValueForIsk () =>
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
