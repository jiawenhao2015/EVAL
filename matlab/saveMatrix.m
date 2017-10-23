function  saveMatrix(filePath,M)
fid=fopen(filePath,'w');
[t1,t2]=size(M);
for ti=1:t1
    for tj=1:t2
        fprintf(fid,'%f',M(ti,tj));
        fprintf(fid,' ');
    end
    fprintf(fid,'\n');
end
fclose(fid);
