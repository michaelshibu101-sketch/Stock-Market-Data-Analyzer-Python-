#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

struct Candle {
    std::string date;
    double open{};
    double high{};
    double low{};
    double close{};
    long long volume{};
};

std::vector<std::string> split(const std::string& line, char delim) {
    std::vector<std::string> out;
    std::stringstream ss(line);
    std::string part;
    while (std::getline(ss, part, delim)) out.push_back(part);
    return out;
}

std::vector<Candle> readCsv(const std::string& path) {
    std::ifstream in(path);
    if (!in) throw std::runtime_error("Could not open file: " + path);

    std::vector<Candle> rows;
    std::string line;
    std::getline(in, line); // header
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        auto cols = split(line, ',');
        if (cols.size() < 6) continue;
        Candle c;
        c.date = cols[0];
        c.open = std::stod(cols[1]);
        c.high = std::stod(cols[2]);
        c.low = std::stod(cols[3]);
        c.close = std::stod(cols[4]);
        c.volume = std::stoll(cols[5]);
        rows.push_back(c);
    }
    return rows;
}

double sma(const std::vector<double>& values, std::size_t endIndex, std::size_t period) {
    if (endIndex + 1 < period) return std::numeric_limits<double>::quiet_NaN();
    double sum = 0.0;
    for (std::size_t i = endIndex + 1 - period; i <= endIndex; ++i) sum += values[i];
    return sum / static_cast<double>(period);
}

double ema(const std::vector<double>& values, std::size_t endIndex, std::size_t period) {
    if (endIndex + 1 < period) return std::numeric_limits<double>::quiet_NaN();
    double multiplier = 2.0 / (period + 1.0);
    double value = sma(values, period - 1, period);
    for (std::size_t i = period; i <= endIndex; ++i) {
        value = (values[i] - value) * multiplier + value;
    }
    return value;
}

double rsi(const std::vector<double>& closes, std::size_t endIndex, std::size_t period) {
    if (endIndex < period) return std::numeric_limits<double>::quiet_NaN();
    double gains = 0.0, losses = 0.0;
    for (std::size_t i = endIndex + 1 - period; i <= endIndex; ++i) {
        double diff = closes[i] - closes[i - 1];
        if (diff > 0) gains += diff;
        else losses -= diff;
    }
    if (losses == 0.0) return 100.0;
    double rs = gains / losses;
    return 100.0 - (100.0 / (1.0 + rs));
}

double volatility(const std::vector<double>& closes, std::size_t endIndex, std::size_t period) {
    if (endIndex + 1 < period) return std::numeric_limits<double>::quiet_NaN();
    std::vector<double> returns;
    for (std::size_t i = endIndex + 1 - period + 1; i <= endIndex; ++i) {
        returns.push_back((closes[i] - closes[i - 1]) / closes[i - 1]);
    }
    double mean = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    double sq = 0.0;
    for (double x : returns) sq += (x - mean) * (x - mean);
    return std::sqrt(sq / returns.size()) * std::sqrt(252.0);
}

std::string signal(double fast, double slow, double rsiValue) {
    if (std::isnan(fast) || std::isnan(slow) || std::isnan(rsiValue)) return "HOLD";
    if (fast > slow && rsiValue < 70.0) return "BUY";
    if (fast < slow && rsiValue > 30.0) return "SELL";
    return "HOLD";
}

int main(int argc, char* argv[]) {
    try {
        std::string path = argc >= 2 ? argv[1] : "sample_prices.csv";
        auto candles = readCsv(path);
        if (candles.size() < 20) {
            std::cerr << "Need at least 20 rows of data.\n";
            return 1;
        }

        std::vector<double> closes;
        closes.reserve(candles.size());
        for (const auto& c : candles) closes.push_back(c.close);

        std::cout << std::left
                  << std::setw(12) << "Date"
                  << std::setw(10) << "Close"
                  << std::setw(10) << "SMA5"
                  << std::setw(10) << "EMA10"
                  << std::setw(10) << "RSI14"
                  << std::setw(12) << "Volatility"
                  << std::setw(8) << "Signal" << '\n';
        std::cout << std::string(72, '-') << '\n';

        for (std::size_t i = 14; i < candles.size(); ++i) {
            double s5 = sma(closes, i, 5);
            double e10 = ema(closes, i, 10);
            double r14 = rsi(closes, i, 14);
            double vol = volatility(closes, i, 10);
            std::cout << std::setw(12) << candles[i].date
                      << std::setw(10) << std::fixed << std::setprecision(2) << candles[i].close
                      << std::setw(10) << s5
                      << std::setw(10) << e10
                      << std::setw(10) << r14
                      << std::setw(12) << vol
                      << std::setw(8) << signal(s5, e10, r14) << '\n';
        }

        const auto& last = candles.back();
        double lastSma = sma(closes, closes.size() - 1, 5);
        double lastEma = ema(closes, closes.size() - 1, 10);
        double lastRsi = rsi(closes, closes.size() - 1, 14);
        std::cout << "\nSummary for " << last.date << ":\n";
        std::cout << "Close: " << last.close << "\n";
        std::cout << "Suggested action: " << signal(lastSma, lastEma, lastRsi) << "\n";
        return 0;
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << '\n';
        return 1;
    }
}
