use warp::Filter;

#[tokio::main]
async fn main() {
    let hello = warp::path::end().map(|| warp::reply::html("Hello!"));
    warp::serve(hello).run(([0, 0, 0, 0], 8000)).await;
}
