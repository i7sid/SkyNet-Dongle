## skynet test filter

library(zoo)

args = commandArgs(trailingOnly=TRUE)

# check command line
if (length(args)==0) {
    stop("Usage: Rscript <scriptname> <in_file> <out_file_base>", call.=FALSE)
}

# interpret command line
in_file = args[1]
out_file_base = args[2]

out_file_raw_a = c(out_file_base, "-raw.csv")
out_file_raw = paste(out_file_raw_a, sep="", collapse="")

out_file_5_a = c(out_file_base, "-5min.csv")
out_file_5 = paste(out_file_5_a, sep="", collapse="")

out_file_15_a = c(out_file_base, "-15min.csv")
out_file_15 = paste(out_file_15_a, sep="", collapse="")

out_file_diff_a = c(out_file_base, "-diff.csv")
out_file_diff = paste(out_file_diff_a, sep="", collapse="")


out_dir_file_raw_a = c(out_file_base, "-dir-raw.csv")
out_dir_file_raw = paste(out_dir_file_raw_a, sep="", collapse="")

out_dir_file_5_a = c(out_file_base, "-dir-5min.csv")
out_dir_file_5 = paste(out_dir_file_5_a, sep="", collapse="")

out_dir_file_15_a = c(out_file_base, "-dir-15min.csv")
out_dir_file_15 = paste(out_dir_file_15_a, sep="", collapse="")

out_dir_file_diff_a = c(out_file_base, "-dir-diff.csv")
out_dir_file_diff = paste(out_dir_file_diff_a, sep="", collapse="")


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

av_short <- rollapplyr(z$V3[,1], list(-(60:1)), mean, fill=0)
av_short_dir <- rollapplyr(z$V4[,1], list(-(60:1)), mean, fill=0)

write.table(av_short, file = out_file_5, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")
write.table(av_short_dir, file = out_dir_file_5, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")


av_long <- rollapplyr(z$V3[,1], list(-(180:1)), mean, fill=0)
av_long_dir <- rollapplyr(z$V4[,1], list(-(180:1)), mean, fill=0)

write.table(av_long, file = out_file_15, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")
write.table(av_long_dir, file = out_dir_file_15, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")



diff = av_short - av_long
diff_dir = av_short_dir - av_long_dir

write.table(diff, file = out_file_diff, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")
write.table(diff_dir, file = out_dir_file_diff, append = FALSE, quote = TRUE, sep = ",",
            eol = "\n", na = "NA", dec = ".", row.names = TRUE,
            col.names = FALSE, qmethod = c("escape", "double"),
            fileEncoding = "UTF-8")



