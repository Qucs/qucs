function qucstrsolversetup(dodebug, verbose)
% compiles the fpproc mexfunction

    if nargin < 1
        dodebug = false;
    end
    
    if nargin < 2
        verbose = false;
    end 

    if isoctave
        cc.Name = 'gcc';
    else
        try
            % First check a C++ compiler is present, and selected
            cc = mex.getCompilerConfigurations('C++', 'Selected');
        catch
            % if the getCompilerConfigurations call fails, try with gcc,
            % assuming that we are on windows and perhaps using gnumex
            cc.Name = 'gcc';
        end
    end

    % store the current directory
    origdir = pwd;

    % change to the mfemm directory (the directory this file is in)
    cd(fileparts(which('qucstrsolversetup.m')));

    % set some common compiler flags, we replace all calls to printf to
    % calls to mexPrintf
    if dodebug
        common_compiler_flags = {'-DHAVE_CONFIG_H', '-g', '-D"_MEX_DEBUG"'};
    else
        common_compiler_flags = {'-DHAVE_CONFIG_H'};% common_compiler_flags = '-D"printf=mexPrintf"';
    end
    
    if verbose
        common_compiler_flags = [common_compiler_flags, {'-v'}];
    end

    % TODO: build the libries?
    libcommands = { ...
                   '../../libqucs-core.a', ...
                   '-I"../"', ... % src directory
                   '-I"../components"',...
                   '-I"../components/devices"',...
                   '-I"../component/digital"',...
                   '-I"../components/microstrip"',...
                   '-I"../components/verilog"',...
                   '-I"../converter"',...
                   '-I"../math"',...
                   '-I"../../"', ... % top level qucs-core directory
                  };

    % put all the compiler commands in a cell array
    mexcommands = [ common_compiler_flags, ...
                    { ...
                      'm_trsolver_interface_mex.cpp', ...
                      'mextrsolver.cpp', ...
                    }, ...
                    libcommands ...
                  ];

    if isoctave
        mkoctfile('--mex', mexcommands{:});
    else
        % call mex with the appropriately constructed commands
        mex(mexcommands{:});
    end

    % return to original directory
    cd(origdir);

end
