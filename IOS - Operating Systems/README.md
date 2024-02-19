# XTF

## February 2024

### Description

Like good technology visionaries, your company has decided to create its own cryptocurrency exchange, called XTF (eXtortion, Travesty, Fraud). The exchange allows its clients to deposit funds in the form of cryptocurrencies and fiat currencies; exchange their funds for other currencies; custody of funds and lending of funds (called margin).

In order for users to keep track of their accounts (and the funds in them), your exchange must keep a record of the transactions made by all users. However, your accounting is very cluttered and messy (as it should be for any good startup), and so all records are mixed up with each other and stored in one or more text, or compressed, files. However, through the GUI, you should only show each user records from their business account. As part of the pre-processing of the data for output, you will need to filter and sort the user records.

However, if you really want to succeed in the field of cryptocurrency fintech companies, you need to grow faster and more than your competitors. In order to lure as many new users as possible to your exchange, you have promised them fabulous appreciation of their deposited funds through your sister investment company. Since financial regulations are for backward-looking people and not for visionaries of your cut, there is nothing stopping you from showing users fictitious profits on their investments.

Although you are otherwise principally concerned with using the latest and most advanced technologies (the right buzzwords will ensure an influx of additional capital from investors), in this particular case it actually makes more sense to solve the problem using a shell script rather than a trained neural network.

### Name

- `xtf` - a script for preprocessing logs from your cryptocurrency exchange.

### Usage

- `xtf [-h|--help] [FILTER] [COMMAND] USER LOG [LOG2 [...]`

### Options

- `COMMAND` can be one of:

  - `list` - a listing of records for the given user.
  - `list-currency` - listing a sorted list of occurring currencies.
  - `status` - a listing of the actual account status grouped and sorted by currency.
  - `profit` - a statement of the customer's account status with notional profit included.

- `FILTER` can be a combination of the following:

  - `-a DATETIME` - after: only records AFTER this date and time (without it) are considered. DATETIME is of format YYYY-MM-DD HH:MM:SS.
  - `-b DATETIME` - before: only records BEFORE this date and time (without it) are considered.
  - `-c CURRENCY` - only records corresponding to the given currency are considered.

- `-h` and `--help` print a help message with a short description of each command and switch.

### Specification

1. The script filters, groups and dumps logs from cryptocurrency exchange logs. If a command is given to the script, it executes the command over the filtered records.
2. If the script is to dump a list or account status, each entry is dumped on one line and only once. The order of the lines is given alphabetically by currency code. Items must not be repeated. The output format for the account status for each currency is `CODE : VALUE`.
3. `list` describes the (filtered) records corresponding to the `USER` on the standard output in the order in which they appear in the input log files.
4. `list-currency` lists the currency codes occurring in the `USER`'s (filtered) records. The currency codes are sorted alphabetically.
5. `status` calculates and prints the account status for each currency of the `USER` based on the (filtered) records. The list of currencies and their balances is sorted alphabetically by currency code. In this case, do not include notional investment returns in the results.
6. `profit` is similar to the `status`, except that positive currency balances are artificially inflated by `x`%, where `x` is the value of the environment variable `XTF_PROFIT`. However, you are a bit forgetful and so sometimes forget to set the variable. If the `XTF_PROFIT` environment variable is not set, consider the default value of 20% (too high an appreciation could attract unwanted attention from regulators). The value of the `XTF_PROFIT` variable will always be a non-negative integer.

### Details

1. If the script receives no command input, consider the default command `list`.
2. The script can also process logs compressed using the `gzip` tool (if the log file name ends in `.gz`). Obviously, without compression it would be difficult to store large amounts of data on your servers.
3. Assume that input files cannot have names corresponding to a command, switch, or currency code.
4. Assume that the value of the input parameter `USER` does not match the value of any command, switch, or currency code.
5. If the `-h` or `--help` switch is specified, it will always just print a hint and exit the script (that is, if the switch is followed by any command or file, it will not execute).
