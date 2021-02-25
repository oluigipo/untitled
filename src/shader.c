typedef uint Shader;
typedef uint Uniform;

Shader shader_load(const char* restrict name) {
	char path[64];
	char info[512];
	char source[65536]; // 64KiB
	const char* sourceptr = source;
	usize len;
	int success;
	
	// Read Vertex Shader
	sprintf(path, "%s.vs", name);
	len = file_read(path, source, sizeof source);
	if (len == sizeof(source) - 1) {
		printf("File is too long... '%s'\n", path);
		return 0;
	}
	
	// Compile Vertex Shader
	uint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &sourceptr, NULL);
	glCompileShader(vertexShader);
	
	// Check for errors
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, sizeof info, NULL, info);
		printf("Vertex Shader Compilation Error: '%s'\n==========================\n%s\n", path, info);
		glDeleteShader(vertexShader);
		return 0;
	}
	
	// Read Fragment Shader
	sprintf(path, "%s.fs", name);
	len = file_read(path, source, sizeof source);
	if (len == sizeof(source) - 1) {
		printf("File is too long... '%s'\n", path);
		glDeleteShader(vertexShader);
		return 0;
	}
	
	// Compile Fragment Shader
	uint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &sourceptr, NULL);
	glCompileShader(fragmentShader);
	
	// Check for errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, sizeof info, NULL, info);
		printf("Fragment Shader Compilation Error: '%s'\n==========================\n%s\n", path, info);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
		return 0;
	}
	
	// Link
	Shader program = glCreateProgram();
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	glLinkProgram(program);
	
	// Check for errors
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(program, sizeof info, NULL, info);
		printf("Shader Linking Error: '%s'\n==========================\n%s\n", name, info);
		glDeleteProgram(program);
		program = 0;
	}
	
	// Clean-up
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	return program;
}

Uniform shader_uniform(Shader shd, const char* name) {
	return glGetUniformLocation(shd, name);
}

void shader_unload(Shader shd) {
	glDeleteProgram(shd);
}

