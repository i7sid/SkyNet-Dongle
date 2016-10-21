## skynet test filter

# configuration
length_very_short   <- 12 
length_short        <- 60   #* (4/5)
length_long         <- 180  # 360
length_very_long    <- 360

library(zoo)

## helpers
rad2deg <- function(rad) {(rad * 180) / (pi)}
deg2rad <- function(deg) {(deg * pi) / (180)}


## numeric differentiation for zoo objects (helper func)
numdifferentiate <- function(z, h) {
  x <- 1
  return(
    (z[x+h]-z[x]) / 
      ((1+h)-1)
  )
}

## numeric differentiation for zoo objects (CALL THIS!)
zoonumdiff <- function(z, h, trimbefore = 1) {
  r <- rollapply(z, (1+h), FUN=numdifferentiate, h, fill=0)
  r[1:trimbefore] <- 0
  return(r)
}


## convert polar to cartesian
wind2cartesian <- function(inp) {
  v <- inp[,1]
  dir <- inp[,2]
  
  x <- v^2 * sin(deg2rad(dir))
  y <- v^2 * cos(deg2rad(dir))
  
  r <- c(x, y)
  m <- matrix(r, ncol=2)
  
  return(m)
}

## convert cartesian to polar
wind2polar <- function(inp) {
  x <- inp[,1]
  y <- inp[,2]
  
  v = sqrt(sqrt(x^2 + y^2))
  dir = (450 - rad2deg(atan2(y, x))) %% 360
  
  r <- c(v, dir)
  m <- matrix(r, ncol=2)
  
  return(m)
}


## own mean function for vectors
vectormean <- function(inp) {
  v = inp[,1]
  dir = inp[,2]
  
  x = v^2 * sin(deg2rad(dir))
  y = v^2 * cos(deg2rad(dir))
  
  x_mean = mean(x)
  y_mean = mean(y)
  
  #v_m = sqrt(x_mean^2 + y_mean^2)
  v_m = sqrt(sqrt(x_mean^2 + y_mean^2))
  dir_m = (450 - rad2deg(atan2(y_mean, x_mean))) %% 360
  
  r = c(v_m, dir_m)
  names(r) = c("V3", "V4")
  return(r)
}


## own difference funtion for direction (treats jumps > 180° as in other direction)
dirdiffelem <- function(z) {
  a <- z[1]
  b <- z[2]
  
  if      (a - b >  180) return(360 - a - b)
  else if (a - b < -180) return(360 + a - b)
  else                   return(a - b)
}


## check command line
args = commandArgs(trailingOnly=TRUE)
if (length(args)==0) {
    stop("Usage: Rscript <scriptname> <in_file> <out_file_base>", call.=FALSE)
}

## interpret command line
in_file = args[1]
out_file_base = args[2]

out_file_raw_a = c(out_file_base, "-raw.csv")
out_file_raw = paste(out_file_raw_a, sep="", collapse="")

out_file_5_a = c(out_file_base, "-5min.csv")
out_file_5 = paste(out_file_5_a, sep="", collapse="")

out_file_2_a = c(out_file_base, "-1min.csv")
out_file_2 = paste(out_file_2_a, sep="", collapse="")

out_file_15_a = c(out_file_base, "-15min.csv")
out_file_15 = paste(out_file_15_a, sep="", collapse="")

out_file_30_a = c(out_file_base, "-30min.csv")
out_file_30 = paste(out_file_30_a, sep="", collapse="")

out_file_diff_a = c(out_file_base, "-diff.csv")
out_file_diff = paste(out_file_diff_a, sep="", collapse="")


out_dir_file_raw_a = c(out_file_base, "-dir-raw.csv")
out_dir_file_raw = paste(out_dir_file_raw_a, sep="", collapse="")

out_dir_file_2_a = c(out_file_base, "-dir-1min.csv")
out_dir_file_2 = paste(out_dir_file_2_a, sep="", collapse="")

out_dir_file_5_a = c(out_file_base, "-dir-5min.csv")
out_dir_file_5 = paste(out_dir_file_5_a, sep="", collapse="")

out_dir_file_15_a = c(out_file_base, "-dir-15min.csv")
out_dir_file_15 = paste(out_dir_file_15_a, sep="", collapse="")

out_dir_file_30_a = c(out_file_base, "-dir-30min.csv")
out_dir_file_30 = paste(out_dir_file_30_a, sep="", collapse="")

out_dir_file_diff_a = c(out_file_base, "-dir-diff.csv")
out_dir_file_diff = paste(out_dir_file_diff_a, sep="", collapse="")


out_file_diff_diff_a = c(out_file_base, "-diff-diff.csv")
out_file_diff_diff = paste(out_file_diff_diff_a, sep="", collapse="")

out_dir_file_diff_diff_a = c(out_file_base, "-dir-diff-diff.csv")
out_dir_file_diff_diff = paste(out_dir_file_diff_diff_a, sep="", collapse="")

out_file_polar_a = c(out_file_base, "-vector.csv")
out_file_polar = paste(out_file_polar_a, sep="", collapse="")

wspeed <- read.csv(file=in_file, head=FALSE, sep=",")
datum <- strptime(wspeed$V1, format="%Y-%m-%d %H:%M:%S")

z <- zoo(wspeed[, 3:4], as.POSIXct(as.character(wspeed[, 1]), format="%Y-%m-%d %H:%M:%S"))
#duplicated(index(z))

write.table(z$V3, file = out_file_raw, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")
write.table(z$V4, file = out_dir_file_raw, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")

av_short <- rollapplyr(z, list(-(length_short:1)), vectormean, fill=0, by.column=FALSE)

write.table(av_short$V3, file = out_file_5, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")
write.table(av_short$V4, file = out_dir_file_5, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")


av_long <- rollapplyr(z, list(-(length_long:1)), vectormean, fill=0, by.column=FALSE)

write.table(av_long$V3, file = out_file_15, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")
write.table(av_long$V4, file = out_dir_file_15, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")



diff = av_short$V3 - av_long$V3

merged_diff <- merge(av_short$V4, av_long$V4)
diff_dir <- zoo(apply(merged_diff, 1, dirdiffelem), time(merged_diff))

## write diff short/long
write.table(diff, file = out_file_diff, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")
write.table(diff_dir, file = out_dir_file_diff, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")


## numerically differentiate
h <- 5
diff_diff <- zoonumdiff(diff, h, 1)
diff_dir_diff <- zoonumdiff(diff_dir, h, 1)

## smooth diffs a bit
smh <- 20
diff_diff <- rollapplyr(diff_diff, list(-(smh:1)), mean, fill=0)
diff_dir_diff <- rollapplyr(diff_dir_diff, list(-(smh:1)), mean, fill=0)


write.table(diff_diff, file = out_file_diff_diff, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")
write.table(diff_dir_diff, file = out_dir_file_diff_diff, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")




## further means
av_very_short <- rollapplyr(z, list(-(length_very_short:1)), vectormean, fill=0, by.column=FALSE)

write.table(av_very_short$V3, file = out_file_2, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")
write.table(av_very_short$V4, file = out_dir_file_2, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")


av_very_long <- rollapplyr(z, list(-(length_very_long:1)), vectormean, fill=0, by.column=FALSE)

write.table(av_very_long$V3, file = out_file_30, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")
write.table(av_very_long$V4, file = out_dir_file_30, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")



indices <- index(z[,1])
z_long <- rollapplyr(z, list(-(length_long:1)), vectormean, fill=0, by.column=FALSE)
z_short <- rollapplyr(z, list(-(length_short:1)), vectormean, fill=0, by.column=FALSE)

z_diff <- z_short - z_long

wind_u_cartesian <- wind2cartesian(coredata(z_long));
z_u_cartesian <- aggregate(zoo(wind_u_cartesian), as.POSIXct(indices), tail, 1)
#plot(z_u_cartesian$V1, z_u_cartesian$V2, xlim=c(-300,300), ylim=c(-300,300))

## calculate updraft vectors
wind_cartesian <- wind2cartesian(coredata(z));
z_cartesian <- aggregate(zoo(wind_cartesian), as.POSIXct(indices), tail, 1)
#plot(z_cartesian$V1, z_cartesian$V2, xlim=c(-300,300), ylim=c(-300,300))

z_a_cartesian <- z_cartesian - z_u_cartesian
#plot(z_a_cartesian$V1, z_a_cartesian$V2, xlim=c(-300,300), ylim=c(-300,300))

wind_a_polar <- wind2polar(coredata(z_a_cartesian))
z_a_polar <- aggregate(zoo(wind_a_polar), as.POSIXct(indices), tail, 1)
z_a_short <- rollapplyr(z_a_polar, list(-(length_short:1)), vectormean, fill=0, by.column=FALSE)


write.zoo(z_a_short, file = out_file_polar, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")



### OLD STUFF, just for reference

## own mean function for direction
# dirmean <- function(inp) {
#   x = sin(deg2rad(inp))
#   y = cos(deg2rad(inp))
#   
#   x_mean = mean(x)
#   y_mean = mean(y)
#   
#   # border case x=y=0 is not important
#   if (x_mean >= 0 && y_mean == 0) {
#     return(90)
#   }
#   else if (x_mean < 0 && y_mean == 0) {
#     return(270)
#   }
#   else if (x_mean >= 0 && y_mean > 0) {
#     return(rad2deg(atan(abs(x_mean)/abs(y_mean))))
#   }
#   else if (x_mean >= 0 && y_mean < 0) {
#     return(180 - rad2deg(atan(abs(x_mean)/abs(y_mean))))
#   }
#   else if (x_mean < 0 && y_mean > 0) {
#     return(360 - rad2deg(atan(abs(x_mean)/abs(y_mean))))
#   }
#   else if (x_mean < 0 && y_mean < 0) {
#     return(180 + rad2deg(atan(abs(x_mean)/abs(y_mean))))
#   }
#   
#   
#   return(0)
# }
# 
