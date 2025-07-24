#include <chrono>
#include <cmath>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

chrono::system_clock::time_point tm_to_timepoint(const tm &date) {
  time_t tt = mktime(const_cast<tm *>(&date));
  return chrono::system_clock::from_time_t(tt);
}

tm timepoint_to_tm(const chrono::system_clock::time_point &tp) {
  time_t tt = chrono::system_clock::to_time_t(tp);
  tm local_tm = *localtime(&tt);
  return local_tm;
}

tm add_months(const tm &date, int months) {
  tm result = date;
  result.tm_mon += months;

  // Normalize the tm structure
  if (result.tm_mon > 11) {
    result.tm_year += result.tm_mon / 12;
    result.tm_mon %= 12;
  } else if (result.tm_mon < 0) {
    result.tm_year += (result.tm_mon - 11) / 12;
    result.tm_mon = (result.tm_mon % 12 + 12) % 12;
  }

  // Normalize the day of the month
  mktime(&result);

  return result;
}

double fraction(string dcc, tm sdate, tm edate) {
  double dcf = 0;
  if (dcc == "ACT/ACT") {
    tm date = sdate;
    while (tm_to_timepoint(date) <= tm_to_timepoint(edate)) {
      if (date.tm_year % 4 == 0) {
        dcf += 1 / 366;
      } else {
        dcf += 1 / 365;
      }
      tm_to_timepoint(date) += chrono::hours(24);
    }
  } else if (dcc == "ACT/360") {
    auto duration = tm_to_timepoint(edate) - tm_to_timepoint(sdate);
    dcf = (chrono::duration_cast<chrono::hours>(duration).count() / 24) / 360;
  } else if (dcc == "ACT/365") {
    auto duration = tm_to_timepoint(edate) - tm_to_timepoint(sdate);
    dcf = (chrono::duration_cast<chrono::hours>(duration).count() / 24) / 365;
  } else if (dcc == "30/360") {
    // check this d1 to 3 calc 30 nov, 1 dec example
    int d1 = 0;
    int d2 = 0;
    int d3 = 0;
    d1 = max(d1, (edate.tm_mday - sdate.tm_mday));
    d2 = max(d2, (edate.tm_mon - sdate.tm_mon));
    d3 = max(d3, (edate.tm_year - sdate.tm_year));
    dcf = (d1 + (d2 * 30) + (d3 * 360)) / 360;
  }
  return dcf;
}

double newtonraphson(vector<double> &payableinterest,
                     vector<double> &yearfraction, double &yield, int counter,
                     int couponfreqfix, int couponfreqflt, double tradeprice) {
  double sum = 0;
  double error = 0;
  for (int i = 0; i < payableinterest.size(); i++) {
    if (i < counter) {
      double dDf = 1 / (1 + yield / (100 * couponfreqfix));
      double pv =
          payableinterest[i] * pow(dDf, yearfraction[i] * couponfreqfix);
      sum += pv;
      double dDer = -1 * 0.01 * dDf * pv * yearfraction[i];
      error += dDer;
    } else {
      double dDf = 1 / (1 + yield / (100 * couponfreqflt));
      double pv =
          payableinterest[i] * pow(dDf, yearfraction[i] * couponfreqflt);
      sum += pv;
      double dDer = -1 * 0.01 * dDf * pv * yearfraction[i];
      error += dDer;
    }
  }
  yield = yield + ((tradeprice - sum) / error);
  return sum;
}

int main() {

  tm issuedate = {};
  tm settlementdate = {};
  tm maturitydate = {};
  tm firstcoupondate = {};
  tm lastcoupondate = {};
  tm startdatefix = {};
  tm startdateflt = {};
  tm enddatefix = {};
  tm enddateflt = {};
  double tradeprice;
  double fv;
  string dccfix, dccflt;
  int couponfreqfix, couponfreqflt;
  double couponfix, couponflt;
  double refrate, spread;
  double bbgyield;

  cout << "Issue Date (YYYY MM DD): " << endl;
  cin >> get_time(&issuedate, "%Y-%m-%d");
  cout << "Settlement Date (YYYY MM DD): " << endl;
  cin >> get_time(&settlementdate, "%Y-%m-%d");
  cout << "Maturity Date (YYYY MM DD): " << endl;
  cin >> get_time(&maturitydate, "%Y-%m-%d");
  cout << "First Coupon Date (YYYY MM DD): " << endl;
  cin >> get_time(&firstcoupondate, "%Y-%m-%d");
  cout << "Last Coupon Date (YYYY MM DD): " << endl;
  cin >> get_time(&lastcoupondate, "%Y-%m-%d");
  cout << "Trade Price: " << endl;
  cin >> tradeprice;
  cout << "Face Value: " << endl;
  cin >> fv;
  // 30/360, ACT/ACT, ACT/360, ACT/365
  cout << "Day Count Convention(FIX): " << endl;
  cin >> dccfix;
  cout << "Day Count Convention(FLT): " << endl;
  cin >> dccflt;
  cout << "Coupon Frequency(FIX): " << endl;
  cin >> couponfreqfix;
  cout << "Coupon Frequency(FLT): " << endl;
  cin >> couponfreqflt;
  cout << "Start Date for FIX (YYYY MM DD): " << endl;
  cin >> get_time(&startdatefix, "%Y-%m-%d");
  cout << "Start Date for FLT (YYYY MM DD): " << endl;
  cin >> get_time(&startdateflt, "%Y-%m-%d");
  cout << "End Date for FIX (YYYY MM DD): " << endl;
  cin >> get_time(&enddatefix, "%Y-%m-%d");
  cout << "End Date for FLT (YYYY MM DD): " << endl;
  cin >> get_time(&enddateflt, "%Y-%m-%d");
  cout << "Coupon (FIX): " << endl;
  cin >> couponfix;
  cout << "Coupon (FLT): " << endl;
  cin >> couponflt;
  cout << "SOFR Reference Rate (FLT): " << endl;
  cin >> refrate;
  cout << "Spread (FLT): " << endl;
  cin >> spread;
  cout << "Bloomberg Yield for reference: " << endl;
  cin >> bbgyield;

  vector<tm> coupondates;
  vector<double> payableinterest;
  vector<double> yearfraction;
  chrono::system_clock::time_point tp = tm_to_timepoint(firstcoupondate);
  chrono::system_clock::time_point sd = tm_to_timepoint(settlementdate);

  while (tp <= sd) {
    tp = tm_to_timepoint(add_months(timepoint_to_tm(tp), (12 / couponfreqfix)));
  }

  tm tp_conv = timepoint_to_tm(tp);

  double accruedinterest =
      couponfix * fraction(dccfix, tp_conv, settlementdate);
  tradeprice = tradeprice + accruedinterest;

  if (tp != sd) {
    tp = tm_to_timepoint(add_months(timepoint_to_tm(tp), (12 / couponfreqfix)));
  }

  chrono::system_clock::time_point fixend = tm_to_timepoint(startdateflt);

  while (tp <= fixend) {
    tp_conv = timepoint_to_tm(tp);
    coupondates.push_back(tp_conv);
    tp = tm_to_timepoint(add_months(tp_conv, (12 / couponfreqfix)));

    double interest =
        couponfix * fraction(dccfix, tp_conv, timepoint_to_tm(tp));
    payableinterest.push_back(interest);
    double yf = fraction(dccfix, settlementdate, timepoint_to_tm(tp));
    yearfraction.push_back(yf);
  }
  int counter = payableinterest.size();
  chrono::system_clock::time_point last =
      tm_to_timepoint(add_months(lastcoupondate, (12 / couponfreqflt)));

  while (tp <= last) {
    tp_conv = timepoint_to_tm(tp);
    coupondates.push_back(tp_conv);
    tp = tm_to_timepoint(add_months(tp_conv, (12 / couponfreqflt)));
    double interest =
        couponfix * fraction(dccflt, tp_conv, timepoint_to_tm(tp));
    if (tp == last) {
      interest = interest + fv;
    }
    payableinterest.push_back(interest);
    double yf = fraction(dccflt, settlementdate, timepoint_to_tm(tp));
    yearfraction.push_back(yf);
  }

  double yield = 0;
  double sum = 0;
  while (true) {
    sum = newtonraphson(payableinterest, yearfraction, yield, counter,
                        couponfreqfix, couponfreqflt, tradeprice);
    if ((sum - tradeprice) <= pow(10, -5) &&
        (sum - tradeprice) >= -pow(10, -5)) {
      break;
    }
  }
  cout << "The YTM is: " << yield << endl;
  return 0;
}
