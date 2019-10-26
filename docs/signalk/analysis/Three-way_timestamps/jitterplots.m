function jitterplots(filename1, filename2, filename3)
  % Jitter analysis and plot
  % Requires: https://octave.sourceforge.io/dataframe/
  % file is the dataframe input file name: ex. 'file.dat'
  % It must be in format where the timestamp is column1
  % For example, from InfluxDB 2.0 output CSV:
  %     #group	false	false	true	true	false	false	true	true	true
  %     #datatype	string	long	dateTime:RFC3339	dateTime:RFC3339	dateTime:RFC3339	double	string	string	string #default	_result								
	%     result	table	_start	_stop	_time	_value	_field	_measurement	prop1
	% 	  0	2019-10-05T16:14:00Z	2019-10-05T16:19:00Z	2019-10-05T16:14:01.169Z	-28.1	angleTrue	environment	wind
	%     0	2019-10-05T16:14:00Z	2019-10-05T16:19:00Z	2019-10-05T16:14:03.106Z	-26.6	angleTrue	environment	wind
  % Make a 'file.dat' like this:
  %     _time,_value
  %     date,double
  %     2019-10-05T16:14:01.169Z,-28.1
  %     2019-10-05T16:14:03.106Z,-26.6
  %
if nargin !=1; end
  pkg load dataframe
df1 = dataframe( filename1 );
df2 = dataframe( filename2 );
df3 = dataframe( filename3 );
% conversion to datenum numeric format from RFC3339 format
z1 = cellfun (@(x) datenum(x, "yyyy-mm-ddTHH:MM:SS.FFFZ"),
     cellstr (df1.array(:,1)));
z2 = cellfun (@(x) datenum(x, "yyyy-mm-ddTHH:MM:SS.FFFZ"),
     cellstr (df2.array(:,1)));
z3 = cellfun (@(x) datenum(x, "yyyy-mm-ddTHH:MM:SS.FFFZ"),
     cellstr (df3.array(:,1)));
% conversion to unix-timestamps in milliseconds since EPOCH
y1 = arrayfun (@(x) round(8.64e7 *
              (x - 1 - datenum('1970', 'yyyy'))), z1);
y2 = arrayfun (@(x) round(8.64e7 *
              (x - 1 - datenum('1970', 'yyyy'))), z2);
y3 = arrayfun (@(x) round(8.64e7 *
              (x - 1 - datenum('1970', 'yyyy'))), z3);

% calculate differences i2-i1
d1 = diff(y1);
d2 = diff(y2);
d3 = diff(y3);
% calculate cumulative integral (trapezoidal)
ci1 = cumtrapz(d1);
ci2 = cumtrapz(d2);
ci3 = cumtrapz(d3);
% plot comparison
subplot(2,3,1);
plot(d1);
xlabel('x');ylabel('diff()');
title(strrep(filename1,"_"," "));
subplot(2,3,4);
plot(ci1);
xlabel('x');ylabel('cumtrapz()');
title(['total: ' num2str(max(ci1)) ' pnts: ' num2str(size(ci1)) ''] );
%
subplot(2,3,2);
plot(d2);
xlabel('x');ylabel('diff()');
title(strrep(filename2,"_"," "));
subplot(2,3,5);
plot(ci2);
xlabel('x');ylabel('cumtrapz()');
title(['total: ' num2str(max(ci2)) ' pnts: ' num2str(size(ci2)) ''] );
%
subplot(2,3,3);
plot(d3);
xlabel('x');ylabel('diff()');
title(strrep(filename3,"_"," "));
subplot(2,3,6);
plot(ci3);
xlabel('x');ylabel('cumtrapz()');
title(['total: ' num2str(max(ci3)) ' pnts: ' num2str(size(ci3)) ''] );
endfunction

