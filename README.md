# Stock Analyzer

A C++17 market analysis tool that reads OHLCV CSV data, calculates technical indicators, and prints a trading summary.

## Why this is a strong resume project
- data processing in C++
- financial analysis logic
- custom implementations of SMA, EMA, RSI, and volatility
- clean terminal reporting and reusable CSV pipeline

## Features
- reads stock price data from CSV
- computes SMA(5), EMA(10), RSI(14), and rolling volatility
- generates a simple BUY / SELL / HOLD suggestion
- easy to swap in a real market data export later

## Build on Windows
```powershell
g++ -std=c++17 -O2 main.cpp -o stock_analyzer.exe
.\stock_analyzer.exe
```

To use another CSV file:
```powershell
.\stock_analyzer.exe your_file.csv
```

## Build on Linux
```bash
g++ -std=c++17 -O2 main.cpp -o stock_analyzer
./stock_analyzer
```

## Suggested resume bullets
- Built a C++ market analytics engine that parses OHLCV datasets and computes SMA, EMA, RSI, and annualized volatility.
- Designed a reusable CSV processing pipeline and rule-based signal engine to classify market conditions as BUY, SELL, or HOLD.
