import yfinance as yf
import os

tickers = ["AAPL", "MSFT", "SPY"]
output_dir = "data"
os.makedirs(output_dir, exist_ok=True)

for ticker in tickers:
    df = yf.download(ticker, start="2015-01-01", end="2025-01-01")
    df = df[["Open", "High", "Low", "Close", "Volume"]]
    path = os.path.join(output_dir, f"{ticker}.csv")
    df.columns = ["Open", "High", "Low", "Close", "Volume"]
    df.to_csv(path)
    print(f"{ticker}: {len(df)} bars -> {path}")

