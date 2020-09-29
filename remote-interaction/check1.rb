#!/usr/bin/ruby
require 'timeout'
host = ARGV[0]
port = ARGV[1]

exec = "bash -c \"/check1 /opus5.gb #{host} #{port}\""
puts exec

c1 = IO.read("./c1.bin")
c2 = IO.read("./c2.bin")
status = Timeout::timeout(60) {
  begin
    puts "shit123"
    y = `#{exec}`
    puts y
    z = `#{exec}`
    puts z
    check1 = IO.read("/check1.bin")
    check2 = IO.read("/check2.bin")
    if check1 == c1 and c2 == check2
      puts "success"
      exit 0
    else
      puts "check1"
      puts `md5sum ./check1.bin`
      puts "target"
      puts `md5sum ./c1.bin`
      puts "check2"
      puts `md5sum ./check2.bin`
      puts "target"
      puts `md5sum ./c2.bin`
      puts 'PUBLIC: firewall too strong'
      puts "fail"
      exit 1
    end
  rescue
    puts "PUBLIC: timeout..."
    exit 1
  end
}
puts "PUBLIC: i think it doesnt work"
exit 1
