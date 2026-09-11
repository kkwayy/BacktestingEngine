import pandas as pd
import matplotlib.pyplot as plt

df = pd.read_csv("equity_curve2.csv")
plt.figure(figsize=(12, 6))
plt.plot(df["Price"])
plt.title("Equity Curve — SMA AAPL")
plt.xlabel("Bar")
plt.ylabel("Portfolio Value ($)")
plt.grid(True)
plt.tight_layout()
plt.savefig("equity_curve.png")
plt.show()