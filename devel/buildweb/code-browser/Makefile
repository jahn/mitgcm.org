

default:
	make dev

release1:
	cd MakeBrowseableMITGCM; ./makebrowseablecodepages.sh release1_p5; cp work/web.tgz ..; rm -f work
	cd ..; tar -xzf code-browser/web.tgz; rm -f code-browser/web.tgz

dev:
	cd MakeBrowseableMITGCM; ./makebrowseablecodepages.sh dev_docs; cp work/web.tgz ..; rm -f work
	cd ..; tar -xzf code-browser/web.tgz; rm -f code-browser/web.tgz


