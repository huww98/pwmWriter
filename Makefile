pwmWriter: pwmWriter.cpp
	$(CXX) -o $@ --std=c++14 pwmWriter.cpp -lwiringPi -pthread -lcrypt -lrt

clean:
	rm pwmWriter

reload-service: pwmWriter.service
	systemctl daemon-reload
	systemctl restart pwmWriter.service

install: pwmWriter
	cp pwmWriter /usr/sbin/pwmWriter
	systemctl enable $(shell pwd)/pwmWriter.service
	systemctl start pwmWriter.service

uninstall:
	systemctl stop pwmWriter.service
	systemctl disable pwmWriter.service
	rm /usr/sbin/pwmWriter

