require 'rspec'

describe 'database' do
  let(:long_username){ "a"*32 }
  let(:long_email){ "a"*255 }

  before :all do
    `make`
  end

  after :all do
    `rm -rf bin`
  end
  # TODO remove magic strings
  def run_script(commands)
    raw_output = nil
    IO.popen("./bin/sqlsmt", "r+") do |pipe|
      commands.each do |command|
        pipe.puts command
      end

      pipe.close_write

      # Read entire output
      raw_output = pipe.gets(nil)
    end
    raw_output.split("\n")
  end

  it 'inserts and retreives a row' do
    result = run_script([
      "insert 1 user1 person1@example.com",
      "select",
      ".exit",
    ])
    expect(result).to match_array([
      "sqlsmt > Executed.",
      "sqlsmt > (1, user1, person1@example.com)",
      "Executed.",
      "sqlsmt > ",
    ])
  end

  it 'prints error msg when table is full' do
    script = (1..1401).map do |i|
      "insert #{i} user#{i} person#{i}@eample.com"
    end
    script<<".exit"
    result = run_script(script)
    expect(result[-2]).to eq('sqlsmt > Error: Table full.')
  end

  it 'allows inserting strings that are the maximum length' do
    script = [
      "insert 1 #{long_username} #{long_email}",
      "select",
      ".exit"
    ]
    result = run_script(script)
    expect(result).to match_array([
                                    "sqlsmt > Executed.",
                                    "sqlsmt > (1, #{long_username}, #{long_email})",
                                    "Executed.",
                                    "sqlsmt > "
                                  ])
  end

  it 'prints error msg if strings are too long' do
    script = [
      "insert 1 #{long_username*2} #{long_email*2}",
      "select",
      ".exit"
    ]
    result = run_script(script)
    expect(result).to match_array([
                                    "sqlsmt > String is too long.",
                                    "sqlsmt > Executed.",
                                    "sqlsmt > "
                                  ])
  end

  it 'prints error when id is negative' do
    script = [
      "insert -1 username username@example.com",
      "select",
      ".exit"
    ]
    result = run_script(script)
    expect(result).to match_array([
                                    "sqlsmt > ID must be positive.",
                                    "sqlsmt > Executed.",
                                    "sqlsmt > "
                                  ])
  end
end
