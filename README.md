# Fixed Income Yield Calculator (FGC Internship)

This repository contains a C++ program for calculating bond yields, developed during my internship at the **Fintech Global Center**. The project implements the **Newton-Raphson method** to accurately solve for various yield metrics for fixed and floating-rate bonds.

---

## Overview

The core of this project is a robust C++ model designed to handle the complexities of real-world bond pricing. It was created to translate theoretical bond mathematics and Excel-based financial models into a precise and efficient C++ application.

Key functionalities include:
* **Yield Calculation**: Implements the Newton-Raphson numerical method to find Yield to Maturity (YTM), Yield to Call (YTC), and Yield to Worst (YTW).
* **Accrued Interest**: Precisely calculates and adjusts for accrued interest across various coupon periods.
* **Day Count Conventions**: Supports multiple day count conventions to ensure accuracy.
* **Bond Types**: Designed to work for both fixed and floating-rate bonds.

---

## Internship Context & Learnings

This project was the practical application of the knowledge I gained during the internship, where my focus was on:
* **Bond Fundamentals**: Understanding coupon structures, duration, credit risk, and bond variations (zero-coupon, callable, convertible).
* **Risk-Return Analysis**: Examining bond prices, yields, and interest rate sensitivity using metrics like **DV01** and **PV01**.
* **Yield Metrics**: Mastering the calculation of **YTM, YTC, YTW, BEY**, and **EAY** using methods like IRR (DCF analysis) in Excel.

---

## Excel Financial Model

This C++ implementation was based on an initial financial model built in Excel. The Excel model uses DCF analysis and the `RATE` function to calculate bond yields.

The Excel file is uploaded in this repo.

---

## How to Compile and Run

1.  **Clone the repository:**
    ```sh
    git clone [https://github.com/your-username/your-repo-name.git](https://github.com/your-username/your-repo-name.git)
    ```
2.  **Navigate to the project directory:**
    ```sh
    cd your-repo-name
    ```
3.  **Compile the C++ file** (assuming the main file is `yield_calculator.cpp`):
    ```sh
    g++ yield_calculator.cpp -o calculator
    ```
4.  **Run the executable:**
    ```sh
    ./calculator
    ```
