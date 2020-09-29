require 'minitcp'
require 'json'
require 'msgpack'
require 'timeout'
require 'pry'

def is_ok? data, cfg
  begin
	  rules = cfg["firewall"]["rules"]
	  max_vals = cfg["max_checks"]
	  max_checks = cfg["max_vals"]
	  data_message = data
    unless rules.nil?
      func = data_message[2]
      rule = nil
      rules.each do |r|
        if func == r["function"]
          rule = r
          break
        end
      end
      if rule["checks"].count > max_checks
        puts "too many checks"
        return true
      end
      if rule["values"].count > max_vals
        puts "too many values"
        return true
      end
      vals = []
      rule["values"].each do |val|
        if val["type"] == "CONST"
          vals << val["value"]
        else
          arg = val["argument"]
          idx = val["idx"]
          arg_vec = data_message[3][arg]
          vals << arg_vec[idx]
        end
      end
      rule["checks"].each do |check|
        v1idx = check["v1"]
        v2idx = check["v2"]
        v1 = vals[v1idx]
        v2 = vals[v2idx]
        op = check["op"]
        if op == "eq"
          if v1 != v2
            return false
          end
        elsif op == "ne"
          if v1 == v2
            return false
          end
        elsif op == "gt"
          if v1 <= v2
            return false
          end
        elsif op == "lt"
          if v1 >= v2
            return false
          end
        end
      end
    end
  rescue
    return true
  end
  return true
end

max_thread = 16
@cfg_meta = JSON.parse(IO.read(ARGV[0]))
@cfgs = []
@cfg_meta["cfgs"].each do |cfg|
  mcfg = JSON.parse(IO.read("#{cfg}.json"))
  mcfg["name"] = cfg
  mcfg["firewall"] = JSON.parse(IO.read(ARGV[1]))[mcfg["name"]]
  puts mcfg
  @cfgs << mcfg
end

def parse_cfg data
  begin
    data_message = MessagePack.unpack(data)
  rescue
    return nil
  end
  service = data_message[2].split("::")[0]
  cfg = nil
  @cfgs.each do |mcfg|
    if service == mcfg["name"]
      cfg = mcfg
    end
  end
  rules = cfg["firewall"]["rules"]
  max_vals = cfg["max_checks"]
  max_checks = cfg["max_vals"]
  return cfg
end

def time_diff_milli(start, finish)
   (finish - start) * 1000.0
end

puts " "

@open = 0
MServer.service(@cfg_meta["listen_port"], '0.0.0.0', max_thread) do |client_socket|
  srv_socket = nil
  du = nil
  client_socket.on_any_receive do |data|
    d = nil
    while true
      begin
	du = MessagePack.unpack(data)
	break
      rescue
        d = client_socket.recv(1024)
	unless d.nil?
		data << d
		next
          break
        end
	puts "FUCK"
        #data << d
      end
    end

    cfg = parse_cfg data

    #while @open > 100
      #sleep 0.1
    #end

    if cfg.nil?
	    binding.pry
    end

    th = MClient.run_one_shot(cfg["remote_host"], cfg["remote_port"]) do |so|
      #puts @open
      @open = @open + 1
      srv_socket = so
      if is_ok? du, cfg
        so.send(data, 0)
      else
        puts "FIREWALL BLOCKED"
      end

      so.on_any_receive do |data|
        client_socket.send(data, 0)
      end
      #so.wait_end
      sleep 10
      so.close
      @open = @open - 1
      #client_socket.close
    end
  end

  unless srv_socket.nil?
    srv_socket.close
  end
  client_socket.wait_end
  client_socket.close
end.join
