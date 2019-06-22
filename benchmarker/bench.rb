#!/usr/bin/env ruby
require "open3"
require 'date'
require "pp"
require "json"
require "fileutils"

class String
  def red; "\e[31m#{self}\e[0m" end
  def green; "\e[32m#{self}\e[0m" end
  def magenta; "\e[35m#{self}\e[0m" end
  def cyan; "\e[36m#{self}\e[0m" end
  def bold; "\e[1m#{self}\e[22m" end
end


__dir__ = File.expand_path("..", __FILE__)
class Bench
  def initialize
    @problems = Dir.glob("problems/part-*/*.desc", base: __dir__)
    @problems.map!{|problem| File.join(__dir__,problem) }

    best_sol = Dir.glob("best/*.sol__*", base: __dir__)
    @best_sol = best_sol.map{|name|
      m = File.basename(name).match(/(.+).sol__(.+)___.+/)
      [m[1], [m[2].to_i,  m[0], File.join(__dir__, name)]]
    }.to_h

    @best_dir = File.absolute_path(File.join(__dir__, "best"))
  end
  def tmp_dir prog
    commit_hash = `git rev-parse HEAD`.chomp[0,8]
    now = DateTime.now.strftime("%y-%m-%dT%H-%M-%S")
    result_dir_name = "#{now}__#{commit_hash}__#{File.basename(prog)}"
    result_dir = File.join(__dir__, "results", result_dir_name)
    Dir.mkdir(result_dir)
    result_dir
  end
  def run_all prog
    result_dir = tmp_dir prog
    puts "Output to #{result_dir}"

    succ = 0
    ng = 0
    @problems.each.with_index{|problem, i|
      problem_name = File.basename(problem, ".desc")
      puts "Problem #{i}/#{@problems.size}: #{problem_name}"
      res = run prog, problem, result_dir
      if res then succ += 1 else ng += 1 end
    }
    puts "Succ: #{succ}, NG: #{ng}"
    puts "./bench.rb verify #{result_dir}".green
    result_dir
  end
  def run prog, problem, result_dir
    problem_name = File.basename(problem, ".desc")
    out, err, status = Open3.capture3 "./#{prog} < #{problem}"
    print err
    if status != 0
      puts "Error status: #{status}".red
      output_name = "#{problem_name}.ERROR"
      output_file = File.join(result_dir, output_name)
      File.write(output_file, err)
      return false
    end

    output_name = "#{problem_name}.sol.pre"
    output_file = File.join(result_dir, output_name)
    File.write(output_file, out)

    true
  end

  def verify_all dir
    sols = Dir.glob("*.sol.pre", base:dir)

    # Interactive Verifier
    cmd = "cd #{__dir__}/verifier && npm run --silent verify"
    Open3.popen2e(cmd){|sin,souterr,wt|
      sin.sync = true
      souterr.sync = true

      sols.each.with_index{|sol, i|
        m = sol.match(/(.+).sol.pre/)
        prob_name = m[1]
        prob = @problems.find{|x|x.end_with?("#{prob_name}.desc")}
        print "#{i}/#{sols.size} Verify #{sol} on #{prob}..."

        sol = File.join(dir, sol)
        json = "{\"desc\":\"#{prob}\",\"sol\":\"#{sol}\"}"
        soltime_name = sol[0...-4]+"__*"
        if ! Dir.glob(soltime_name).empty?
          puts "already verified"
          next
        end

        # Write Output
        sin.puts json
        sin.flush
        res = souterr.readline
        time = verify_json res

        if time # Get time or false
          soltime_name = sol[0...-4]+"__#{time}"
          FileUtils.copy(sol, soltime_name) 
          best = @best_sol[prob_name]
          besttime, litbesttime = if best
            [best[0], "#{best[0]} by #{best[1]}"]
          else
            [999999999999999999999, "No Sol"]
          end
          res = "Result: #{time} (best: #{litbesttime})"
          result_dir_name = File.basename(File.dirname(soltime_name))
          best_name = File.basename("#{soltime_name}___#{result_dir_name}")
          best_file = File.join(@best_dir, best_name)
          if time < besttime
            res = "#{res} 🎉".green
            puts "copy #{soltime_name} -> #{best_file}"
            FileUtils.copy(soltime_name, best_file)
            if best
              puts "remove #{best[2]}"
              File.unlink(best[2])
            end
          end
          puts res
        else
          puts "Failed".red
        end
      }
    }
  end

  def verify_json out
    res = JSON.parse(out)
    if res["status"] == "OK"
      puts "OK: #{res["time"]}".green
      return res["time"].to_i
    else
      puts res["msg"].red
      return false
    end
  end

  def verify prob, sol
    cmd = "cd #{__dir__}/verifier && npm run --silent verify #{prob} #{sol}"
    out, err, status = Open3.capture3 cmd
    verify_json out
  end

  def run_verify prog, prob
    problem = @problems.find{|f| f.end_with?(prob)}
    puts "Start #{problem} on #{prog}"
    result_dir = tmp_dir prog
    res = run prog, problem, result_dir
    puts "Exec OK".green
    if res
      sol = Dir.glob(File.join(result_dir,"*"))[0]
      verify problem, sol
    end
  end
end

case cmd = ARGV[0]
when "run"
  unless prog = ARGV[1]
    raise "./bench.rb run PROG.a"
  end
  Bench.new.run_all prog
when "verify"
  unless dir = ARGV[1]
    raise "./bench.rb verify ./2019-06-22T17:31:25+09:00__0dbc30c4__guchoku.o"
  end
  Bench.new.verify_all dir
when "check"
  unless prog = ARGV[1]
    raise "./bench.rb check PROG.a prob-XXX.desc"
  end
  unless prob = ARGV[2]
    raise "./bench.rb check PROG.a prob-XXX.desc"
  end
  Bench.new.run_verify prog, prob
when "genzip"
  Dir.glob("bestzip/*", base:__dir__).each{|f|
    f = File.join(__dir__, f)
    puts "Remove #{f}"
    FileUtils.remove(f)
  }
  Dir.glob("best/*sol__*", base:__dir__).each{|f|
    to = File.join(__dir__, "bestzip", File.basename(f.match(/(.+\.sol)__.+/)[1]))
    f = File.join(__dir__, f)
    puts "Add #{f}\n-> #{to}"
    FileUtils.copy(f, to)
  }
  zipdir = File.join(__dir__, "bestzip")
  zipname = File.join(__dir__, "bestzip.zip")
  puts "Generate zip #{zipdir} --> #{zipname}"
  FileUtils.remove(zipname) if File.exist?(zipname)
  `cd #{zipdir}; zip -r #{zipname} ./*.sol`
else
  puts "
./bench.rb run {prog.o} : プログラムを実行してディレクトリに吐く
./bench.rb verify {dir} : ディレクトリの検査をしてbestをアップデートをする
./bench.rb genzip : bestをzipに固めて bestzip.zipを作る
./bench.rb check {prog.o} {prob-XXX.desc} : 単一実行して検査 bestは更新しない (verifyすれば更新される)
 "
  raise "./bench.rb run | verify | genzip | check"
end