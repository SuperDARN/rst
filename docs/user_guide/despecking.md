<!--
(C) copyright 2022 University Centre in Svalbard (UNIS)
author: Emma Bland, UNIS
-->
# Despecking fitacf files

Hardware and operational factors sometimes affect the noise level estimation in FitACF, resulting in contamination of the fitted data with "salt and pepper noise". 

This contamination can be identified in range-time plots as data points that are isolated in range and time:

![!](figures/salt_and_pepper_noise.png)

The `fit_speck_removal` routine is designed to remove these isolated points from the fitted data. Despecking is performed separately for each beam and each channel. The output is a fitacf file with the salt and pepper noise removed, but otherwise identical to the input file. 

## Usage

After generating a `fitacf` file using `make_fit`, the despecking routine can be applied as follows:

```
fit_speck_removal [inputfile].fitacf > [outputfile].despeck.fitacf
```


## Considerations for use

Users are encouraged to view the fitted data before applying the despecking procedure. Note that `fit_speck_removal` is likely to remove considerable amounts of **meteor scatter** and other small populations of backscatter that have a limited temporal and spatial extent. 

It is also not recommended to use this routine with **multi-frequency data** that have not been separated into different frequency channels (e.g., **two-frequency data** from SAS, PGR, RKN, INV and CLY generated before December 2016).


## Example

The plots below show the fitted data before and after the despecking procedure. Notice that the isolated points in range gates ~40 and above have been removed, as well as a large amount of the meteor scatter at near ranges. 

![!](figures/timeplot_pwr_20180308_cly.fitacf3.png)
![!](figures/timeplot_pwr_20180308_cly.despeck.fitacf3.png)

