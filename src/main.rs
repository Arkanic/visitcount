use std::convert::Infallible;

use serde_derive::{Deserialize, Serialize};
use warp::{Filter, http::StatusCode, reject::Rejection, reply::Reply};

#[derive(Serialize)]
struct CountResult {
    success:bool,
    count:u128
}

#[derive(Serialize)]
struct RequestFail {
    success:bool,
    message:String
}

#[derive(Deserialize)]
struct CountInput {
    hash:u128
}

#[tokio::main]
async fn main() {
    let countresult = warp::post()
        .and(warp::path("count"))
        .and(warp::body::content_length_limit(1024 * 4))
        .and(warp::body::json())
        .map(|mut request:CountInput| {
            let result = CountResult {
                count: 50,
                success: true
            };
            warp::reply::json(&result)
        })
        .recover(reject_handle);
    warp::serve(countresult).run(([127, 0, 0, 1], 3030)).await
}

async fn reject_handle(_:Rejection) -> Result<impl Reply, Infallible> {
    Ok(warp::reply::with_status(
        warp::reply::json(&RequestFail {
            success: false,
            message: "bad request".to_string()
        }),
        StatusCode::BAD_REQUEST
    ))
}